#pragma once

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "error.h"
#include "expr.h"
// #include "stmt.h"
#include "token.h"
#include "token_type.h"

/**
 * Recursive descent parser
 * Transforms tokens into an abstract syntax tree
 */
class Parser
{
private:
    // Custom exception for parser errors with recovery
    struct ParseError : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    // Parser state
    const std::vector<Token> &token_stream;
    int current_pos = 0;

public:
    /**
     * Constructs a parser with the given token stream
     */
    Parser(const std::vector<Token> &tokens)
        : token_stream{tokens}
    {
    }

    // /**
    //  * Parse all statements in the token stream
    //  * @return Vector of parsed statements
    //  */
    // std::vector<std::shared_ptr<Stmt>> parse()
    // {
    //     std::vector<std::shared_ptr<Stmt>> program_statements;

    //     // Parse all statements until end of file
    //     while (!is_at_end())
    //     {
    //         program_statements.push_back(declaration());
    //     }

    //     return program_statements;
    // }

    std::shared_ptr<Expr> parse()
    {
        try
        {
            return expression();
        }
        catch (ParseError error)
        {
            return nullptr;
        }
    }

private:
    // //---------------------------------------------
    // // Statement parsing methods
    // //---------------------------------------------

    // /**
    //  * Parse a declaration statement
    //  */
    // std::shared_ptr<Stmt> declaration()
    // {
    //     try
    //     {
    //         if (match(VAR))
    //         {
    //             return variable_declaration();
    //         }
    //         return statement();
    //     }
    //     catch (ParseError &error)
    //     {
    //         // Error recovery
    //         recover_from_error();
    //         return nullptr;
    //     }
    // }

    // /**
    //  * Parse a regular statement
    //  */
    // std::shared_ptr<Stmt> statement()
    // {
    //     if (match(FOR))
    //         return for_statement();
    //     if (match(IF))
    //         return if_statement();
    //     if (match(PRINT))
    //         return print_statement();
    //     if (match(WHILE))
    //         return while_statement();
    //     if (match(LEFT_BRACE))
    //         return std::make_shared<Block>(block());

    //     return expression_statement();
    // }

    // /**
    //  * Parse a for loop statement
    //  */
    // std::shared_ptr<Stmt> for_statement()
    // {
    //     consume(LEFT_PAREN, "Expect '(' after 'for'.");

    //     // Parse initializer clause
    //     std::shared_ptr<Stmt> init_clause;
    //     if (match(SEMICOLON))
    //     {
    //         init_clause = nullptr;
    //     }
    //     else if (match(VAR))
    //     {
    //         init_clause = variable_declaration();
    //     }
    //     else
    //     {
    //         init_clause = expression_statement();
    //     }

    //     // Parse condition expression
    //     std::shared_ptr<Expr> condition_expr = nullptr;
    //     if (!check(SEMICOLON))
    //     {
    //         condition_expr = expression();
    //     }
    //     consume(SEMICOLON, "Expect ';' after loop condition.");

    //     // Parse increment expression
    //     std::shared_ptr<Expr> increment_expr = nullptr;
    //     if (!check(RIGHT_PAREN))
    //     {
    //         increment_expr = expression();
    //     }
    //     consume(RIGHT_PAREN, "Expect ')' after for clauses.");

    //     // Parse loop body
    //     std::shared_ptr<Stmt> loop_body = statement();

    //     // Desugar for loop into while loop structure

    //     // Add increment to end of body if it exists
    //     if (increment_expr != nullptr)
    //     {
    //         loop_body = std::make_shared<Block>(
    //             std::vector<std::shared_ptr<Stmt>>{
    //                 loop_body,
    //                 std::make_shared<Expression>(increment_expr)});
    //     }

    //     // Create while loop with condition (or true if none provided)
    //     if (condition_expr == nullptr)
    //     {
    //         condition_expr = std::make_shared<Literal>(true);
    //     }
    //     loop_body = std::make_shared<While>(condition_expr, loop_body);

    //     // Add initializer before while loop if it exists
    //     if (init_clause != nullptr)
    //     {
    //         loop_body = std::make_shared<Block>(
    //             std::vector<std::shared_ptr<Stmt>>{init_clause, loop_body});
    //     }

    //     return loop_body;
    // }

    // /**
    //  * Parse an if statement
    //  */
    // std::shared_ptr<Stmt> if_statement()
    // {
    //     consume(LEFT_PAREN, "Expect '(' after 'if'.");
    //     std::shared_ptr<Expr> condition_expr = expression();
    //     consume(RIGHT_PAREN, "Expect ')' after if condition.");

    //     std::shared_ptr<Stmt> then_branch = statement();
    //     std::shared_ptr<Stmt> else_branch = nullptr;

    //     if (match(ELSE))
    //     {
    //         else_branch = statement();
    //     }

    //     return std::make_shared<If>(condition_expr, then_branch, else_branch);
    // }

    // /**
    //  * Parse a print statement
    //  */
    // std::shared_ptr<Stmt> print_statement()
    // {
    //     std::shared_ptr<Expr> value_expr = expression();
    //     consume(SEMICOLON, "Expect ';' after value.");
    //     return std::make_shared<Print>(value_expr);
    // }

    // /**
    //  * Parse a variable declaration
    //  */
    // std::shared_ptr<Stmt> variable_declaration()
    // {
    //     Token var_name = consume(IDENTIFIER, "Expect variable name.");

    //     std::shared_ptr<Expr> init_expr = nullptr;
    //     if (match(EQUAL))
    //     {
    //         init_expr = expression();
    //     }

    //     consume(SEMICOLON, "Expect ';' after variable declaration.");
    //     return std::make_shared<Var>(std::move(var_name), init_expr);
    // }

    // /**
    //  * Parse a while statement
    //  */
    // std::shared_ptr<Stmt> while_statement()
    // {
    //     consume(LEFT_PAREN, "Expect '(' after 'while'.");
    //     std::shared_ptr<Expr> condition_expr = expression();
    //     consume(RIGHT_PAREN, "Expect ')' after condition.");
    //     std::shared_ptr<Stmt> body_stmt = statement();

    //     return std::make_shared<While>(condition_expr, body_stmt);
    // }

    // /**
    //  * Parse an expression statement
    //  */
    // std::shared_ptr<Stmt> expression_statement()
    // {
    //     std::shared_ptr<Expr> expr_value = expression();
    //     consume(SEMICOLON, "Expect ';' after expression.");
    //     return std::make_shared<Expression>(expr_value);
    // }

    // /**
    //  * Parse a block of statements
    //  */
    // std::vector<std::shared_ptr<Stmt>> block()
    // {
    //     std::vector<std::shared_ptr<Stmt>> statements;

    //     // Parse statements until end of block or EOF
    //     while (!check(RIGHT_BRACE) && !is_at_end())
    //     {
    //         statements.push_back(declaration());
    //     }

    //     consume(RIGHT_BRACE, "Expect '}' after block.");
    //     return statements;
    // }

    //---------------------------------------------
    // Expression parsing methods - recursive descent
    //---------------------------------------------

    /**
     * Parse an expression
     */
    std::shared_ptr<Expr> expression()
    {
        return assignment();
    }

    /**
     * Parse an assignment expression
     */
    std::shared_ptr<Expr> assignment()
    {
        std::shared_ptr<Expr> expr = or_expression();

        if (match(EQUAL))
        {
            Token equals_token = previous();
            std::shared_ptr<Expr> right_value = assignment();

            // Ensure left side is a valid assignment target
            if (Variable *var_expr = dynamic_cast<Variable *>(expr.get()))
            {
                Token var_name = var_expr->var_name;
                return std::make_shared<Assign>(std::move(var_name), right_value);
            }

            // Report error but don't throw to avoid cascading errors
            error(std::move(equals_token), "Invalid assignment target.");
        }

        return expr;
    }

    /**
     * Parse logical OR expression
     */
    std::shared_ptr<Expr> or_expression()
    {
        std::shared_ptr<Expr> expr = and_expression();

        while (match(OR))
        {
            Token operator_token = previous();
            std::shared_ptr<Expr> right_term = and_expression();
            expr = std::make_shared<Logical>(expr, std::move(operator_token), right_term);
        }

        return expr;
    }

    /**
     * Parse logical AND expression
     */
    std::shared_ptr<Expr> and_expression()
    {
        std::shared_ptr<Expr> expr = equality();

        while (match(AND))
        {
            Token operator_token = previous();
            std::shared_ptr<Expr> right_term = equality();
            expr = std::make_shared<Logical>(expr, std::move(operator_token), right_term);
        }

        return expr;
    }

    /**
     * Parse equality expression (==, !=)
     */
    std::shared_ptr<Expr> equality()
    {
        std::shared_ptr<Expr> expr = comparison();

        while (match(BANG_EQUAL, EQUAL_EQUAL))
        {
            Token operator_token = previous();
            std::shared_ptr<Expr> right_term = comparison();
            expr = std::make_shared<Binary>(expr, std::move(operator_token), right_term);
        }

        return expr;
    }

    /**
     * Parse comparison expression (<, <=, >, >=)
     */
    std::shared_ptr<Expr> comparison()
    {
        std::shared_ptr<Expr> expr = term();

        while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL))
        {
            Token operator_token = previous();
            std::shared_ptr<Expr> right_term = term();
            expr = std::make_shared<Binary>(expr, std::move(operator_token), right_term);
        }

        return expr;
    }

    /**
     * Parse addition/subtraction expression
     */
    std::shared_ptr<Expr> term()
    {
        std::shared_ptr<Expr> expr = factor();

        while (match(MINUS, PLUS))
        {
            Token operator_token = previous();
            std::shared_ptr<Expr> right_factor = factor();
            expr = std::make_shared<Binary>(expr, std::move(operator_token), right_factor);
        }

        return expr;
    }

    /**
     * Parse multiplication/division expression
     */
    std::shared_ptr<Expr> factor()
    {
        std::shared_ptr<Expr> expr = unary();

        while (match(SLASH, STAR))
        {
            Token operator_token = previous();
            std::shared_ptr<Expr> right_unary = unary();
            expr = std::make_shared<Binary>(expr, std::move(operator_token), right_unary);
        }

        return expr;
    }

    /**
     * Parse unary expression (!, -)
     */
    std::shared_ptr<Expr> unary()
    {
        if (match(BANG, MINUS))
        {
            Token operator_token = previous();
            std::shared_ptr<Expr> right_expr = unary();
            return std::make_shared<Unary>(std::move(operator_token), right_expr);
        }

        return primary();
    }

    /**
     * Parse primary expression (literals, variables, grouping)
     */
    std::shared_ptr<Expr> primary()
    {
        // Boolean literals
        if (match(FALSE))
        {
            return std::make_shared<Literal>(false);
        }

        if (match(TRUE))
        {
            return std::make_shared<Literal>(true);
        }

        // Nil literal
        if (match(NIL))
        {
            return std::make_shared<Literal>(nullptr);
        }

        // Number or string literal
        if (match(NUMBER, STRING))
        {
            return std::make_shared<Literal>(previous().value);
        }

        // Variable reference
        if (match(IDENTIFIER))
        {
            return std::make_shared<Variable>(previous());
        }

        // Grouping expression
        if (match(LEFT_PAREN))
        {
            std::shared_ptr<Expr> inner_expr = expression();
            consume(RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_shared<Grouping>(inner_expr);
        }

        // Error case
        throw error(peek(), "Expect expression.");
    }

    //---------------------------------------------
    // Helper methods
    //---------------------------------------------

    /**
     * Check if current token matches any of the given types
     */
    template <class... T>
    bool match(T... types)
    {
        assert((... && std::is_same_v<T, TokenType>));

        if ((... || check(types)))
        {
            advance();
            return true;
        }

        return false;
    }

    /**
     * Consume current token if it matches expected type,
     * otherwise throw error
     */
    Token consume(TokenType expected_type, std::string_view error_msg)
    {
        if (check(expected_type))
        {
            return advance();
        }

        throw error(peek(), error_msg);
    }

    /**
     * Check if current token is of given type
     */
    bool check(TokenType type)
    {
        if (is_at_end())
        {
            return false;
        }
        return peek().type == type;
    }

    /**
     * Advance to next token and return previous
     */
    Token advance()
    {
        if (!is_at_end())
        {
            current_pos++;
        }
        return previous();
    }

    /**
     * Check if parser has reached end of input
     */
    bool is_at_end()
    {
        return peek().type == END_OF_FILE;
    }

    /**
     * Get current token without consuming
     */
    Token peek()
    {
        return token_stream.at(current_pos);
    }

    /**
     * Get previous token
     */
    Token previous()
    {
        return token_stream.at(current_pos - 1);
    }

    /**
     * Create error at given token
     */
    ParseError error(const Token &token, std::string_view message)
    {
        ::error(token, message);
        return ParseError{""};
    }

    /**
     * Synchronize parser state after error
     */
    void recover_from_error()
    {
        advance();

        // Skip tokens until we find a statement boundary
        while (!is_at_end())
        {
            // Stop if we just passed a semicolon
            if (previous().type == SEMICOLON)
            {
                return;
            }

            // Stop if we're at a statement boundary
            switch (peek().type)
            {
            case CLASS:
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINT:
            case RETURN:
                return;
            }

            advance();
        }
    }
};