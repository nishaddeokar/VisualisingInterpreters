#pragma once

#include <any>
#include <cassert>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include "expr.h"
#include "stmt.h"

/**
 * AST Visualiser - Creates GraphViz dot representations of abstract syntax trees
 * Implements both expression and statement visitors with improved node formatting
 */
class AstPrinter : public ExprVisitor, public StmtVisitor
{
private:
    // Colour constants for different node types
    const std::string CONTROL_COLOUR = "#c8e6fe";  // Light blue for control structures, statements, operations
    const std::string VARIABLE_COLOUR = "#a7fe9c"; // Light green for variables
    const std::string CONSTANT_COLOUR = "#fefdc9"; // Light yellow for constants

    // For generating unique node IDs
    int node_counter = 0;

    // The DOT file content
    std::ostringstream dot_output;

    // Initialise the DOT file structure
    void init_graph()
    {
        node_counter = 0;
        dot_output.str("");
        dot_output.clear();
        dot_output << "digraph AST {\n";
        dot_output << "  node [shape=box, fontname=\"Arial\", fontsize=10];\n";
    }

    // Finalise the DOT file
    void finalise_graph()
    {
        dot_output << "}\n";
    }

    // Format floating point numbers to avoid excessive decimals
    std::string format_number(double value)
    {
        std::ostringstream oss;

        // Check if the value is a whole number
        if (value == static_cast<int>(value))
        {
            oss << static_cast<int>(value);
        }
        else
        {
            // Format with up to 2 decimal places, removing trailing zeros
            oss << std::fixed << std::setprecision(2) << value;
            std::string str = oss.str();

            // Remove trailing zeros and decimal point if necessary
            if (str.find('.') != std::string::npos)
            {
                str = str.substr(0, str.find_last_not_of('0') + 1);
                if (str.back() == '.')
                    str.pop_back();
            }
            return str;
        }
        return oss.str();
    }

    // Create a new node with multi-line label and colour
    std::string create_node(const std::string &label, const std::string &colour = "white")
    {
        std::string node_id = "node" + std::to_string(node_counter++);
        dot_output << "  " << node_id << " [label=\"" << escape_label(label)
                   << "\", style=\"filled\", fillcolor=\"" << colour << "\"];\n";
        return node_id;
    }

    // Create an edge between two nodes
    void create_edge(const std::string &from_id, const std::string &to_id)
    {
        dot_output << "  " << from_id << " -> " << to_id << ";\n";
    }

    // Escape special characters in labels
    std::string escape_label(const std::string &text)
    {
        std::string result;
        for (char c : text)
        {
            if (c == '"' || c == '\\')
            {
                result.push_back('\\');
            }
            result.push_back(c);
        }
        return result;
    }

    // Convert any value to string for visualisation with improved formatting
    std::string any_to_string(const std::any &value)
    {
        if (value.type() == typeid(nullptr))
        {
            return "nil";
        }
        else if (value.type() == typeid(std::string))
        {
            return "\"" + std::any_cast<std::string>(value) + "\"";
        }
        else if (value.type() == typeid(double))
        {
            return format_number(std::any_cast<double>(value));
        }
        else if (value.type() == typeid(bool))
        {
            return std::any_cast<bool>(value) ? "true" : "false";
        }
        return "unknown";
    }

    void generate_output(const std::string &base_filename)
    {
        // Write DOT file
        std::string dot_file = "images\\" + base_filename + ".dot";
        std::string png_file = "images\\" + base_filename + ".png";

        std::ofstream out_file(dot_file);
        out_file << dot_output.str();
        out_file.close();

        // Since dot.exe is in the same folder as our executable, get that path
        std::string dot_cmd;
        dot_cmd = "lib\\dot.exe";

        // Run the command
        std::string cmd = dot_cmd + " -Tpng \"" + dot_file + "\" -o \"" + png_file + "\"";
        int result = system(cmd.c_str());

        if (result == 0)
        {
// Open the image (platform-specific)
#ifdef _WIN32
            cmd = "start " + png_file;
#elif __APPLE__
            cmd = "open " + png_file;
#else
            cmd = "xdg-open " + png_file;
#endif
            system(cmd.c_str());

            std::cout << "AST visualisation created: " << png_file << std::endl;
        }
        else
        {
            std::cerr << "Failed to generate visualisation. Make sure GraphViz is installed." << std::endl;
        }
    }

public:
    void visualise_expr(std::shared_ptr<Expr> expr, const std::string &output_base = "ast_expr")
    {
        init_graph();
        std::any root_id = expr->accept(*this);
        finalise_graph();
        generate_output(output_base);
    }

    void visualise_stmt(std::shared_ptr<Stmt> stmt, const std::string &output_base = "ast_stmt")
    {
        init_graph();
        std::any root_id = stmt->accept(*this);
        finalise_graph();
        generate_output(output_base);
    }

    void visualise_program(const std::vector<std::shared_ptr<Stmt>> &stmts,
                           const std::string &output_base = "ast_program")
    {
        init_graph();

        // Create a special root node for the program
        std::string program_node = create_node("Program", CONTROL_COLOUR);

        // Connect each statement to the program node
        for (const auto &stmt : stmts)
        {
            std::string stmt_node = std::any_cast<std::string>(stmt->accept(*this));
            create_edge(program_node, stmt_node);
        }

        finalise_graph();
        generate_output(output_base);
    }

    std::string print(std::shared_ptr<Expr> expr)
    {
        init_graph();
        std::string root_id = std::any_cast<std::string>(expr->accept(*this));
        finalise_graph();
        return dot_output.str();
    }

    //----------------------------------------------
    // Expression Visitor Methods
    //----------------------------------------------
    std::any visit_assign_expr(std::shared_ptr<Assign> expr) override
    {
        // Create multi-line node for assignment
        std::string label = "Assign\nname: " + expr->var_name.lexeme;
        std::string assign_node = create_node(label, CONTROL_COLOUR);

        // Create node for value and connect
        std::string value_node = std::any_cast<std::string>(expr->expr_value->accept(*this));
        create_edge(assign_node, value_node);

        return assign_node;
    }

    std::any visit_binary_expr(std::shared_ptr<Binary> expr) override
    {
        // Create multi-line node for binary operator
        std::string label = "Binary\noperator: " + expr->operator_token.lexeme;
        std::string op_node = create_node(label, CONTROL_COLOUR);

        // Create nodes for left and right operands and connect
        std::string left_node = std::any_cast<std::string>(expr->left_expr->accept(*this));
        std::string right_node = std::any_cast<std::string>(expr->right_expr->accept(*this));

        create_edge(op_node, left_node);
        create_edge(op_node, right_node);

        return op_node;
    }

    std::any visit_grouping_expr(std::shared_ptr<Grouping> expr) override
    {
        // Simple node for grouping
        std::string group_node = create_node("Grouping", CONTROL_COLOUR);

        // Create node for inner expression and connect
        std::string inner_node = std::any_cast<std::string>(expr->inner_expr->accept(*this));
        create_edge(group_node, inner_node);

        return group_node;
    }

    std::any visit_literal_expr(std::shared_ptr<Literal> expr) override
    {
        // Create multi-line node for literal with its value
        std::string label = "Literal\nvalue: " + any_to_string(expr->literal_value);
        return create_node(label, CONSTANT_COLOUR);
    }

    std::any visit_logical_expr(std::shared_ptr<Logical> expr) override
    {
        // Create multi-line node for logical operator
        std::string label = "Logical\noperator: " + expr->operator_token.lexeme;
        std::string logic_node = create_node(label, CONTROL_COLOUR);

        // Create nodes for left and right operands and connect
        std::string left_node = std::any_cast<std::string>(expr->left_expr->accept(*this));
        std::string right_node = std::any_cast<std::string>(expr->right_expr->accept(*this));

        create_edge(logic_node, left_node);
        create_edge(logic_node, right_node);

        return logic_node;
    }

    std::any visit_unary_expr(std::shared_ptr<Unary> expr) override
    {
        // Create multi-line node for unary operator
        std::string label = "Unary\noperator: " + expr->operator_token.lexeme;
        std::string unary_node = create_node(label, CONTROL_COLOUR);

        // Create node for operand and connect
        std::string operand_node = std::any_cast<std::string>(expr->operand->accept(*this));
        create_edge(unary_node, operand_node);

        return unary_node;
    }

    std::any visit_variable_expr(std::shared_ptr<Variable> expr) override
    {
        // Create multi-line node for variable reference
        std::string label = "Variable\nname: " + expr->var_name.lexeme;
        return create_node(label, VARIABLE_COLOUR);
    }

    //----------------------------------------------
    // Statement Visitor Methods
    //----------------------------------------------
    std::any visit_block_stmt(std::shared_ptr<Block> stmt) override
    {
        // Create node for block
        std::string block_node = create_node("Block", CONTROL_COLOUR);

        // Create nodes for each statement in block and connect
        for (const auto &statement : stmt->statements)
        {
            if (statement)
            {
                std::string stmt_node = std::any_cast<std::string>(statement->accept(*this));
                create_edge(block_node, stmt_node);
            }
        }

        return block_node;
    }

    std::any visit_expression_stmt(std::shared_ptr<Expression> stmt) override
    {
        // Create node for expression statement
        std::string expr_stmt_node = create_node("ExprStmt", CONTROL_COLOUR);

        // Create node for the expression and connect
        std::string expr_node = std::any_cast<std::string>(stmt->expression->accept(*this));
        create_edge(expr_stmt_node, expr_node);

        return expr_stmt_node;
    }

    std::any visit_if_stmt(std::shared_ptr<If> stmt) override
    {
        // Create node for if statement
        std::string if_node = create_node("If", CONTROL_COLOUR);

        // Create nodes for condition, then branch, else branch and connect
        std::string cond_node = std::any_cast<std::string>(stmt->condition->accept(*this));
        create_edge(if_node, cond_node);

        std::string then_node = std::any_cast<std::string>(stmt->then_branch->accept(*this));
        create_edge(if_node, then_node);

        if (stmt->else_branch)
        {
            std::string else_node = std::any_cast<std::string>(stmt->else_branch->accept(*this));
            create_edge(if_node, else_node);
        }

        return if_node;
    }

    std::any visit_print_stmt(std::shared_ptr<Print> stmt) override
    {
        // Create node for print statement
        std::string print_node = create_node("Print", CONTROL_COLOUR);

        // Create node for expression and connect
        std::string expr_node = std::any_cast<std::string>(stmt->expression->accept(*this));
        create_edge(print_node, expr_node);

        return print_node;
    }

    std::any visit_var_stmt(std::shared_ptr<Var> stmt) override
    {
        // Create multi-line node for variable declaration
        std::string label = "Var\nname: " + stmt->name.lexeme;
        std::string var_node = create_node(label, VARIABLE_COLOUR);

        // Create node for initialiser if present
        if (stmt->initialiser)
        {
            std::string init_node = std::any_cast<std::string>(stmt->initialiser->accept(*this));
            create_edge(var_node, init_node);
        }

        return var_node;
    }

    std::any visit_while_stmt(std::shared_ptr<While> stmt) override
    {
        // Create node for while statement
        std::string while_node = create_node("While", CONTROL_COLOUR);

        // Create nodes for condition and body and connect
        std::string cond_node = std::any_cast<std::string>(stmt->condition->accept(*this));
        create_edge(while_node, cond_node);

        std::string body_node = std::any_cast<std::string>(stmt->body->accept(*this));
        create_edge(while_node, body_node);

        return while_node;
    }
};
