#pragma once

#include <any>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>
#include <typeinfo>
#include <sstream>
#include "environment.h"
#include "error.h"
#include "expr.h"
#include "runtime_error.h"
#include "stmt.h"

/**
 * Executes the parsed abstract syntax tree by implementing
 * the visitor pattern for expressions and statements
 */
class Interpreter : public ExprVisitor, public StmtVisitor
{
private:
    // Current execution environment
    std::shared_ptr<Environment> current_env{new Environment};

    /**
     * Converts any value to its string representation
     */
    std::string to_string(const std::any &value)
    {
        // Handle nil values
        if (value.type() == typeid(nullptr))
        {
            return "nil";
        }

        // Handle numeric values
        if (value.type() == typeid(double))
        {
            std::string num_str = std::to_string(std::any_cast<double>(value));

            // Remove trailing .0 for integer values
            if (num_str.size() >= 2 &&
                num_str[num_str.length() - 2] == '.' &&
                num_str[num_str.length() - 1] == '0')
            {
                return num_str.substr(0, num_str.length() - 2);
            }
            return num_str;
        }

        // Handle strings
        if (value.type() == typeid(std::string))
        {
            return std::any_cast<std::string>(value);
        }

        // Handle booleans
        if (value.type() == typeid(bool))
        {
            return std::any_cast<bool>(value) ? "true" : "false";
        }

        // Unknown type
        return "Error in to_string: unrecognised value type";
    }

    /**
     * Determines if a value is truthy in Lox semantics
     */
    bool is_truthy(const std::any &value)
    {
        // nil is falsey
        if (value.type() == typeid(nullptr))
        {
            return false;
        }

        // booleans are their value
        if (value.type() == typeid(bool))
        {
            return std::any_cast<bool>(value);
        }

        // all other values are truthy
        return true;
    }

    /**
     * Compares two values for equality
     */
    bool is_equal(const std::any &left, const std::any &right)
    {
        // nil equality check
        if (left.type() == typeid(nullptr) && right.type() == typeid(nullptr))
        {
            return true;
        }
        if (left.type() == typeid(nullptr) || right.type() == typeid(nullptr))
        {
            return false;
        }

        // String comparison
        if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
        {
            return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
        }

        // Number comparison
        if (left.type() == typeid(double) && right.type() == typeid(double))
        {
            return std::any_cast<double>(left) == std::any_cast<double>(right);
        }

        // Boolean comparison
        if (left.type() == typeid(bool) && right.type() == typeid(bool))
        {
            return std::any_cast<bool>(left) == std::any_cast<bool>(right);
        }

        // Different types are never equal
        return false;
    }

    /**
     * Validates that an operand is a number
     */
    void validate_number_operand(const Token &operator_token, const std::any &operand)
    {
        if (operand.type() == typeid(double))
        {
            return;
        }

        throw RuntimeError{operator_token, "Operand must be a number."};
    }

    /**
     * Validates that both operands are numbers
     */
    void validate_number_operands(const Token &operator_token,
                                  const std::any &left,
                                  const std::any &right)
    {
        bool both_numbers = left.type() == typeid(double) &&
                            right.type() == typeid(double);

        if (both_numbers)
        {
            return;
        }

        throw RuntimeError{operator_token, "Operands must be numbers."};
    }

    /**
     * Evaluates an expression and returns its value
     */
    std::any eval_expression(std::shared_ptr<Expr> expr)
    {
        return expr->accept(*this);
    }

    /**
     * Executes a statement
     */
    void exec_statement(std::shared_ptr<Stmt> stmt)
    {
        stmt->accept(*this);
    }

public:
    /**
     * Executes a list of statements in a new environment scope
     */
    void exec_block(const std::vector<std::shared_ptr<Stmt>> &statements,
                    std::shared_ptr<Environment> block_env)
    {
        // Store previous environment
        std::shared_ptr<Environment> previous_env = this->current_env;

        try
        {
            // Switch to block environment
            this->current_env = block_env;

            // Execute all statements in block
            for (const auto &stmt : statements)
            {
                exec_statement(stmt);
            }
        }
        catch (...)
        {
            // Restore previous environment even if exception occurs
            this->current_env = previous_env;
            throw; // Re-throw the exception
        }

        // Restore previous environment
        this->current_env = previous_env;
    }

    /**
     * Main entry point - interprets a program of statements
     */
    void interpret(const std::vector<std::shared_ptr<Stmt>> &statements)
    {
        try
        {
            // Execute each statement in sequence
            for (const auto &stmt : statements)
            {
                exec_statement(stmt);
            }
        }
        catch (RuntimeError &error)
        {
            // Report runtime errors
            runtime_error(error);
        }
    }

    //-----------------------------------------------
    // Statement Visitor Methods
    //-----------------------------------------------

    /**
     * Executes a block statement
     */
    std::any visit_block_stmt(std::shared_ptr<Block> stmt) override
    {
        exec_block(stmt->statements,
                   std::make_shared<Environment>(current_env));
        return {};
    }

    /**
     * Executes an expression statement
     */
    std::any visit_expression_stmt(std::shared_ptr<Expression> stmt) override
    {
        eval_expression(stmt->expression);
        return {};
    }

    /**
     * Executes an if statement with optional else branch
     */
    std::any visit_if_stmt(std::shared_ptr<If> stmt) override
    {
        // Evaluate condition
        if (is_truthy(eval_expression(stmt->condition)))
        {
            // Execute then branch
            exec_statement(stmt->then_branch);
        }
        else if (stmt->else_branch != nullptr)
        {
            // Execute else branch if it exists
            exec_statement(stmt->else_branch);
        }
        return {};
    }

    /**
     * Executes a print statement
     */
    std::any visit_print_stmt(std::shared_ptr<Print> stmt) override
    {
        // Evaluate expression and convert to string
        std::any result = eval_expression(stmt->expression);
        std::cout << to_string(result) << "\n";
        return {};
    }

    /**
     * Executes a variable declaration
     */
    std::any visit_var_stmt(std::shared_ptr<Var> stmt) override
    {
        // Evaluate initialiser if present, otherwise nil
        std::any initial_value = nullptr;
        if (stmt->initialiser != nullptr)
        {
            initial_value = eval_expression(stmt->initialiser);
        }

        // Define variable in current environment
        current_env->define(stmt->name.lexeme, std::move(initial_value));
        return {};
    }

    /**
     * Executes a while loop
     */
    std::any visit_while_stmt(std::shared_ptr<While> stmt) override
    {
        // Loop until condition is falsey
        while (is_truthy(eval_expression(stmt->condition)))
        {
            exec_statement(stmt->body);
        }
        return {};
    }

    //-----------------------------------------------
    // Expression Visitor Methods
    //-----------------------------------------------

    /**
     * Evaluates variable assignment
     */
    std::any visit_assign_expr(std::shared_ptr<Assign> expr) override
    {
        // Evaluate right-hand side
        std::any value = eval_expression(expr->expr_value);

        // Assign to variable in environment
        current_env->assign(expr->var_name, value);
        return value;
    }

    /**
     * Evaluates a binary expression
     */
    std::any visit_binary_expr(std::shared_ptr<Binary> expr) override
    {
        // Evaluate both operands
        std::any left_value = eval_expression(expr->left_expr);
        std::any right_value = eval_expression(expr->right_expr);

        // Process according to operator type
        switch (expr->operator_token.type)
        {
        // Comparison operators
        case GREATER:
            validate_number_operands(expr->operator_token, left_value, right_value);
            return std::any_cast<double>(left_value) > std::any_cast<double>(right_value);

        case GREATER_EQUAL:
            validate_number_operands(expr->operator_token, left_value, right_value);
            return std::any_cast<double>(left_value) >= std::any_cast<double>(right_value);

        case LESS:
            validate_number_operands(expr->operator_token, left_value, right_value);
            return std::any_cast<double>(left_value) < std::any_cast<double>(right_value);

        case LESS_EQUAL:
            validate_number_operands(expr->operator_token, left_value, right_value);
            return std::any_cast<double>(left_value) <= std::any_cast<double>(right_value);

        // Equality operators
        case EQUAL_EQUAL:
            return is_equal(left_value, right_value);

        case BANG_EQUAL:
            return !is_equal(left_value, right_value);

        // Arithmetic operators
        case MINUS:
            validate_number_operands(expr->operator_token, left_value, right_value);
            return std::any_cast<double>(left_value) - std::any_cast<double>(right_value);

        case SLASH:
            validate_number_operands(expr->operator_token, left_value, right_value);
            return std::any_cast<double>(left_value) / std::any_cast<double>(right_value);

        case STAR:
            validate_number_operands(expr->operator_token, left_value, right_value);
            return std::any_cast<double>(left_value) * std::any_cast<double>(right_value);

        case PLUS:
            // Handle number addition
            if (left_value.type() == typeid(double) && right_value.type() == typeid(double))
            {
                return std::any_cast<double>(left_value) + std::any_cast<double>(right_value);
            }

            // Handle string concatenation
            if (left_value.type() == typeid(std::string) && right_value.type() == typeid(std::string))
            {
                return std::any_cast<std::string>(left_value) + std::any_cast<std::string>(right_value);
            }

            // Error for invalid operands
            throw RuntimeError{expr->operator_token, "Operands must be two numbers or two strings."};
        }

        // Unreachable, but needed to avoid compiler warnings
        return {};
    }

    /**
     * Evaluates a grouping expression
     */
    std::any visit_grouping_expr(std::shared_ptr<Grouping> expr) override
    {
        return eval_expression(expr->inner_expr);
    }

    /**
     * Evaluates a literal value
     */
    std::any visit_literal_expr(std::shared_ptr<Literal> expr) override
    {
        return expr->literal_value;
    }

    /**
     * Evaluates a logical expression with short-circuit evaluation
     */
    std::any visit_logical_expr(std::shared_ptr<Logical> expr) override
    {
        // Evaluate left operand first
        std::any left_result = eval_expression(expr->left_expr);

        // Short-circuit based on operator type
        if (expr->operator_token.type == OR)
        {
            // For OR, if left is truthy, return it without evaluating right
            if (is_truthy(left_result))
            {
                return left_result;
            }
        }
        else
        {
            // For AND, if left is falsey, return it without evaluating right
            if (!is_truthy(left_result))
            {
                return left_result;
            }
        }

        // Otherwise evaluate and return right operand
        return eval_expression(expr->right_expr);
    }

    /**
     * Evaluates a unary expression
     */
    std::any visit_unary_expr(std::shared_ptr<Unary> expr) override
    {
        // Evaluate the operand
        std::any operand_value = eval_expression(expr->operand);

        // Apply the unary operator
        switch (expr->operator_token.type)
        {
        case BANG:
            // Logical NOT
            return !is_truthy(operand_value);

        case MINUS:
            // Numeric negation
            validate_number_operand(expr->operator_token, operand_value);
            return -std::any_cast<double>(operand_value);
        }

        // Unreachable, but needed to avoid compiler warnings
        return {};
    }

    /**
     * Evaluates a variable reference
     */
    std::any visit_variable_expr(std::shared_ptr<Variable> expr) override
    {
        return current_env->get(expr->var_name);
    }
};
