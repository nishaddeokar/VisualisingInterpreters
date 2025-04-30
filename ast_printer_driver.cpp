#include "ast_printer.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Function declarations for each example
void simple_expression();
void complex_expression();
void simple_statement();
void complex_statement();
void short_program();
void long_program();

int main(int argc, char *argv[])
{
    int choice = 0;

    // If command line argument provided, use it as the choice
    if (argc > 1)
    {
        try
        {
            choice = std::stoi(argv[1]);
        }
        catch (...)
        {
            choice = 0;
        }
    }

    // If no valid choice from command line, prompt user
    if (choice < 1 || choice > 6)
    {
        std::cout << "Select an AST visualisation example:\n"
                  << "1. Simple Expression (5 + 3)\n"
                  << "2. Complex Nested Expression ((2 * (3 + 4)) > (8 - 3) && !false)\n"
                  << "3. Simple Statement (print \"Hello, world!\")\n"
                  << "4. Complex Statement (if-else)\n"
                  << "5. Short Program (variable and print)\n"
                  << "6. Longer Program (variables, if, while)\n"
                  << "Enter your choice (1-6): ";
        std::cin >> choice;
    }

    // Execute the chosen example
    switch (choice)
    {
    case 1:
        simple_expression();
        break;
    case 2:
        complex_expression();
        break;
    case 3:
        simple_statement();
        break;
    case 4:
        complex_statement();
        break;
    case 5:
        short_program();
        break;
    case 6:
        long_program();
        break;
    default:
        std::cout << "Invalid choice. Please run again with a number from 1-6.\n";
        return 1;
    }

    return 0;
}

// Example 1: Simple expression (5 + 3)
void simple_expression()
{
    std::cout << "Visualizing simple expression: 5 + 3\n";

    // Create the AST for 5 + 3
    std::shared_ptr<Expr> expression = std::make_shared<Binary>(
        std::make_shared<Literal>(5.0),
        Token{PLUS, "+", nullptr, 1},
        std::make_shared<Literal>(3.0));

    // Visualise the expression
    AstPrinter printer;
    printer.visualise_expr(expression, "simple_expression_ast");
}

void complex_expression()
{
    std::cout << "Visualizing complex nested expression: ((2 * (3 + 4)) > (8 - 3) && !false)\n";

    // Build (3 + 4)
    auto inner_sum = std::make_shared<Binary>(
        std::make_shared<Literal>(3.0),
        Token{PLUS, "+", nullptr, 1},
        std::make_shared<Literal>(4.0));
    auto grouped_inner_sum = std::make_shared<Grouping>(inner_sum);

    // Build (2 * (3 + 4))
    auto left_product = std::make_shared<Binary>(
        std::make_shared<Literal>(2.0),
        Token{STAR, "*", nullptr, 1},
        grouped_inner_sum);
    auto grouped_left_product = std::make_shared<Grouping>(left_product);

    // Build (8 - 3)
    auto right_diff = std::make_shared<Binary>(
        std::make_shared<Literal>(8.0),
        Token{MINUS, "-", nullptr, 1},
        std::make_shared<Literal>(3.0));
    auto grouped_right_diff = std::make_shared<Grouping>(right_diff);

    // Build ((2 * (3 + 4)) > (8 - 3))
    auto comparison = std::make_shared<Binary>(
        grouped_left_product,
        Token{GREATER, ">", nullptr, 1},
        grouped_right_diff);

    // Build !false
    auto not_false = std::make_shared<Unary>(
        Token{BANG, "!", nullptr, 1},
        std::make_shared<Literal>(false));

    // Build ((2 * (3 + 4)) > (8 - 3) && !false)
    auto logical_expr = std::make_shared<Logical>(
        comparison,
        Token{AND, "&&", nullptr, 1},
        not_false);

    // Visualise the expression
    AstPrinter printer;
    printer.visualise_expr(std::make_shared<Grouping>(logical_expr), "complex_expression_ast");
}

// Example 3: Simple statement (print "Hello, world!")
void simple_statement()
{
    std::cout << "Visualizing simple statement: print \"Hello, world!\";\n";

    // Create print statement with string literal
    auto print_stmt = std::make_shared<Print>(
        std::make_shared<Literal>(std::string("Hello, world!")));

    // Visualise the statement
    AstPrinter printer;
    printer.visualise_stmt(print_stmt, "simple_statement_ast");
}

// Example 4: Complex statement (if-else with block)
void complex_statement()
{
    std::cout << "Visualizing complex statement: if (x > 10) { print \"greater\"; } else { print \"smaller\"; }\n";

    // Create variable reference x
    auto var_expr = std::make_shared<Variable>(Token{IDENTIFIER, "x", "x", 1});

    // Create condition x > 10
    auto condition = std::make_shared<Binary>(
        var_expr,
        Token{GREATER, ">", nullptr, 1},
        std::make_shared<Literal>(10.0));

    // Create then branch with block: { print "greater"; }
    std::vector<std::shared_ptr<Stmt>> then_stmts;
    then_stmts.push_back(std::make_shared<Print>(
        std::make_shared<Literal>(std::string("greater"))));
    auto then_block = std::make_shared<Block>(then_stmts);

    // Create else branch with block: { print "smaller"; }
    std::vector<std::shared_ptr<Stmt>> else_stmts;
    else_stmts.push_back(std::make_shared<Print>(
        std::make_shared<Literal>(std::string("smaller"))));
    auto else_block = std::make_shared<Block>(else_stmts);

    // Create if statement
    auto if_stmt = std::make_shared<If>(condition, then_block, else_block);

    // Visualise the statement
    AstPrinter printer;
    printer.visualise_stmt(if_stmt, "complex_statement_ast");
}

// Example 5: Short program (variable declaration and print)
void short_program()
{
    std::cout << "Visualizing short program:\n"
              << "var message = \"Hello\";\n"
              << "print message;\n";

    std::vector<std::shared_ptr<Stmt>> program;

    // var message = "Hello";
    auto var_decl = std::make_shared<Var>(
        Token{IDENTIFIER, "message", "message", 1},
        std::make_shared<Literal>(std::string("Hello")));
    program.push_back(var_decl);

    // print message;
    auto print_stmt = std::make_shared<Print>(
        std::make_shared<Variable>(Token{IDENTIFIER, "message", "message", 2}));
    program.push_back(print_stmt);

    // Visualise the program
    AstPrinter printer;
    printer.visualise_program(program, "short_program_ast");
}

// Example 6: Longer program (variables, if, while)
void long_program()
{
    std::cout << "Visualizing longer program:\n"
              << "var x = 10;\n"
              << "var factorial = 1;\n"
              << "if (x > 0) {\n"
              << "    while (x > 1) {\n"
              << "        factorial = factorial * x;\n"
              << "        x = x - 1;\n"
              << "    }\n"
              << "    print factorial;\n"
              << "} else {\n"
              << "    print \"Cannot compute factorial\";\n"
              << "}\n";

    std::vector<std::shared_ptr<Stmt>> program;

    // var x = 10;
    auto var_x = std::make_shared<Var>(
        Token{IDENTIFIER, "x", "x", 1},
        std::make_shared<Literal>(10.0));
    program.push_back(var_x);

    // var factorial = 1;
    auto var_factorial = std::make_shared<Var>(
        Token{IDENTIFIER, "factorial", "factorial", 2},
        std::make_shared<Literal>(1.0));
    program.push_back(var_factorial);

    // Create while loop body:
    std::vector<std::shared_ptr<Stmt>> while_body;

    // factorial = factorial * x;
    auto multiply = std::make_shared<Binary>(
        std::make_shared<Variable>(Token{IDENTIFIER, "factorial", "factorial", 4}),
        Token{STAR, "*", nullptr, 4},
        std::make_shared<Variable>(Token{IDENTIFIER, "x", "x", 4}));
    auto assign_factorial = std::make_shared<Expression>(
        std::make_shared<Assign>(
            Token{IDENTIFIER, "factorial", "factorial", 4},
            multiply));
    while_body.push_back(assign_factorial);

    // x = x - 1;
    auto decrement = std::make_shared<Binary>(
        std::make_shared<Variable>(Token{IDENTIFIER, "x", "x", 5}),
        Token{MINUS, "-", nullptr, 5},
        std::make_shared<Literal>(1.0));
    auto assign_x = std::make_shared<Expression>(
        std::make_shared<Assign>(
            Token{IDENTIFIER, "x", "x", 5},
            decrement));
    while_body.push_back(assign_x);

    // while (x > 1) { ... }
    auto while_condition = std::make_shared<Binary>(
        std::make_shared<Variable>(Token{IDENTIFIER, "x", "x", 3}),
        Token{GREATER, ">", nullptr, 3},
        std::make_shared<Literal>(1.0));
    auto while_stmt = std::make_shared<While>(
        while_condition,
        std::make_shared<Block>(while_body));

    // Build the then branch block: { while(...) {...}; print factorial; }
    std::vector<std::shared_ptr<Stmt>> then_stmts;
    then_stmts.push_back(while_stmt);

    // print factorial;
    auto print_factorial = std::make_shared<Print>(
        std::make_shared<Variable>(Token{IDENTIFIER, "factorial", "factorial", 7}));
    then_stmts.push_back(print_factorial);

    // Build the else branch: { print "Cannot compute factorial"; }
    std::vector<std::shared_ptr<Stmt>> else_stmts;
    auto print_error = std::make_shared<Print>(
        std::make_shared<Literal>(std::string("Cannot compute factorial")));
    else_stmts.push_back(print_error);

    // if (x > 0) { ... } else { ... }
    auto if_condition = std::make_shared<Binary>(
        std::make_shared<Variable>(Token{IDENTIFIER, "x", "x", 3}),
        Token{GREATER, ">", nullptr, 3},
        std::make_shared<Literal>(0.0));
    auto if_stmt = std::make_shared<If>(
        if_condition,
        std::make_shared<Block>(then_stmts),
        std::make_shared<Block>(else_stmts));

    program.push_back(if_stmt);

    // Visualise the program
    AstPrinter printer;
    printer.visualise_program(program, "long_program_ast");
}
