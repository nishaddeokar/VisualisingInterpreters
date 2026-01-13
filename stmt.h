#pragma once

#include <any>
#include <memory>
#include <utility>
#include <vector>
#include "token.h"
#include "expr.h"

// Forward declarations of statement types
struct Block;
struct Expression;
struct If;
struct Print;
struct Var;
struct While;

/**
 * Visitor interface for processing statement nodes
 * Implements the visitor design pattern for statements
 */
struct StmtVisitor
{
    // Visit methods for each statement type
    virtual std::any visit_block_stmt(std::shared_ptr<Block> stmt) = 0;
    virtual std::any visit_expression_stmt(std::shared_ptr<Expression> stmt) = 0;
    virtual std::any visit_if_stmt(std::shared_ptr<If> stmt) = 0;
    virtual std::any visit_print_stmt(std::shared_ptr<Print> stmt) = 0;
    virtual std::any visit_var_stmt(std::shared_ptr<Var> stmt) = 0;
    virtual std::any visit_while_stmt(std::shared_ptr<While> stmt) = 0;

    // Virtual destructor
    virtual ~StmtVisitor() = default;
};

/**
 * Base class for all statement types
 */
struct Stmt
{
    // Accept method to implement visitor pattern
    virtual std::any accept(StmtVisitor &visitor) = 0;

    // Virtual destructor
    virtual ~Stmt() = default;
};

/**
 * Represents a block of statements
 * Example: { stmt1; stmt2; }
 */
struct Block : Stmt, public std::enable_shared_from_this<Block>
{
    // Member variable
    const std::vector<std::shared_ptr<Stmt>> statements;

    // Constructor
    Block(std::vector<std::shared_ptr<Stmt>> stmt_list)
        : statements{std::move(stmt_list)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visit_block_stmt(shared_from_this());
    }
};

/**
 * Represents an expression statement
 * Example: expression;
 */
struct Expression : Stmt, public std::enable_shared_from_this<Expression>
{
    // Member variable
    const std::shared_ptr<Expr> expression;

    // Constructor
    Expression(std::shared_ptr<Expr> expr)
        : expression{std::move(expr)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visit_expression_stmt(shared_from_this());
    }
};

/**
 * Represents a conditional statement
 * Example: if (condition) thenStmt else elseStmt
 */
struct If : Stmt, public std::enable_shared_from_this<If>
{
    // Member variables
    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Stmt> then_branch;
    const std::shared_ptr<Stmt> else_branch;

    // Constructor
    If(std::shared_ptr<Expr> cond_expr,
       std::shared_ptr<Stmt> then_stmt,
       std::shared_ptr<Stmt> else_stmt)
        : condition{std::move(cond_expr)},
          then_branch{std::move(then_stmt)},
          else_branch{std::move(else_stmt)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visit_if_stmt(shared_from_this());
    }
};

/**
 * Represents a print statement
 * Example: print expression;
 */
struct Print : Stmt, public std::enable_shared_from_this<Print>
{
    // Member variable
    const std::shared_ptr<Expr> expression;

    // Constructor
    Print(std::shared_ptr<Expr> expr)
        : expression{std::move(expr)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visit_print_stmt(shared_from_this());
    }
};

/**
 * Represents a variable declaration
 * Example: var name = initialiser;
 */
struct Var : Stmt, public std::enable_shared_from_this<Var>
{
    // Member variables
    const Token name;
    const std::shared_ptr<Expr> initialiser;

    // Constructor
    Var(Token var_name, std::shared_ptr<Expr> init_expr)
        : name{std::move(var_name)},
          initialiser{std::move(init_expr)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visit_var_stmt(shared_from_this());
    }
};

/**
 * Represents a while loop statement
 * Example: while (condition) body
 */
struct While : Stmt, public std::enable_shared_from_this<While>
{
    // Member variables
    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Stmt> body;

    // Constructor
    While(std::shared_ptr<Expr> cond_expr, std::shared_ptr<Stmt> loop_body)
        : condition{std::move(cond_expr)},
          body{std::move(loop_body)}
    {
    }

    // Implementation of visitor pattern
    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visit_while_stmt(shared_from_this());
    }
};
