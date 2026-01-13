#pragma once

#include <any>
#include <memory>
#include <utility>
#include <vector>
#include "token.h"

// Forward declarations of expression types
struct Assign;
struct Binary;
struct Grouping;
struct Literal;
struct Logical;
struct Unary;
struct Variable;

/**
 * Visitor interface for processing expression nodes
 * Implements the visitor design pattern for expressions
 */
struct ExprVisitor
{
    // Visit methods for each expression type
    virtual std::any visit_assign_expr(std::shared_ptr<Assign> expr) = 0;
    virtual std::any visit_binary_expr(std::shared_ptr<Binary> expr) = 0;
    virtual std::any visit_grouping_expr(std::shared_ptr<Grouping> expr) = 0;
    virtual std::any visit_literal_expr(std::shared_ptr<Literal> expr) = 0;
    virtual std::any visit_logical_expr(std::shared_ptr<Logical> expr) = 0;
    virtual std::any visit_unary_expr(std::shared_ptr<Unary> expr) = 0;
    virtual std::any visit_variable_expr(std::shared_ptr<Variable> expr) = 0;

    // Virtual destructor
    virtual ~ExprVisitor() = default;
};

/**
 * Base class for all expression types
 */
struct Expr
{
    // Accept method to implement visitor pattern
    virtual std::any accept(ExprVisitor &visitor) = 0;
    // Virtual destructor
    virtual ~Expr() = default;
};

/**
 * Represents a variable assignment expression
 * Example: a = 5
 */
struct Assign : Expr, public std::enable_shared_from_this<Assign>
{
    // Member variables
    const Token var_name;
    const std::shared_ptr<Expr> expr_value;

    // Constructor
    Assign(Token name, std::shared_ptr<Expr> value)
        : var_name{std::move(name)}, expr_value{std::move(value)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(ExprVisitor &visitor) override
    {
        return visitor.visit_assign_expr(shared_from_this());
    }
};

/**
 * Represents a binary operation expression
 * Example: a + b, x * y, etc.
 */
struct Binary : Expr, public std::enable_shared_from_this<Binary>
{
    // Member variables
    const std::shared_ptr<Expr> left_expr;
    const Token operator_token;
    const std::shared_ptr<Expr> right_expr;

    // Constructor
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left_expr{std::move(left)},
          operator_token{std::move(op)},
          right_expr{std::move(right)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(ExprVisitor &visitor) override
    {
        return visitor.visit_binary_expr(shared_from_this());
    }
};

/**
 * Represents a parenthesised expression
 * Example: (a + b)
 */
struct Grouping : Expr, public std::enable_shared_from_this<Grouping>
{
    // Member variable
    const std::shared_ptr<Expr> inner_expr;

    // Constructor
    Grouping(std::shared_ptr<Expr> expression)
        : inner_expr{std::move(expression)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(ExprVisitor &visitor) override
    {
        return visitor.visit_grouping_expr(shared_from_this());
    }
};

/**
 * Represents a literal value expression
 * Example: 123, "hello", true
 */
struct Literal : Expr, public std::enable_shared_from_this<Literal>
{
    // Member variable
    const std::any literal_value;

    // Constructor
    Literal(std::any val)
        : literal_value{std::move(val)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(ExprVisitor &visitor) override
    {
        return visitor.visit_literal_expr(shared_from_this());
    }
};

/**
 * Represents a logical operation expression
 * Example: a and b, x or y
 */
struct Logical : Expr, public std::enable_shared_from_this<Logical>
{
    // Member variables
    const std::shared_ptr<Expr> left_expr;
    const Token operator_token;
    const std::shared_ptr<Expr> right_expr;

    // Constructor
    Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left_expr{std::move(left)},
          operator_token{std::move(op)},
          right_expr{std::move(right)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(ExprVisitor &visitor) override
    {
        return visitor.visit_logical_expr(shared_from_this());
    }
};

/**
 * Represents a unary operation expression
 * Example: !a, -b
 */
struct Unary : Expr, public std::enable_shared_from_this<Unary>
{
    // Member variables
    const Token operator_token;
    const std::shared_ptr<Expr> operand;

    // Constructor
    Unary(Token op, std::shared_ptr<Expr> right)
        : operator_token{std::move(op)}, operand{std::move(right)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(ExprVisitor &visitor) override
    {
        return visitor.visit_unary_expr(shared_from_this());
    }
};

/**
 * Represents a variable reference expression
 * Example: foo, bar
 */
struct Variable : Expr, public std::enable_shared_from_this<Variable>
{
    // Member variable
    const Token var_name;

    // Constructor
    Variable(Token name)
        : var_name{std::move(name)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(ExprVisitor &visitor) override
    {
        return visitor.visit_variable_expr(shared_from_this());
    }
};
