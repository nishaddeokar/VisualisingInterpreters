#pragma once

#include <any>
#include <cassert>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include "expr.h"

class AstPrinter : public ExprVisitor
{
public:
    std::string print(std::shared_ptr<Expr> expr)
    {
        return std::any_cast<std::string>(expr->accept(*this));
    }

    std::any visit_binary_expr(std::shared_ptr<Binary> expr) override
    {
        return parenthesize(expr->operator_token.lexeme,
                            expr->left_expr, expr->right_expr);
    }

    std::any visit_grouping_expr(
        std::shared_ptr<Grouping> expr) override
    {
        return parenthesize("group", expr->inner_expr);
    }

    std::any visit_literal_expr(std::shared_ptr<Literal> expr) override
    {
        auto &value_type = expr->literal_value.type();

        if (value_type == typeid(nullptr))
        {
            return "nil";
        }
        else if (value_type == typeid(std::string))
        {
            return std::any_cast<std::string>(expr->literal_value);
        }
        else if (value_type == typeid(double))
        {
            return std::to_string(std::any_cast<double>(expr->literal_value));
        }
        else if (value_type == typeid(bool))
        {
            return std::any_cast<bool>(expr->literal_value) ? "true" : "false";
        }

        return "Error in visitLiteralExpr: literal type not recognized.";
    }

    std::any visit_unary_expr(std::shared_ptr<Unary> expr) override
    {
        return parenthesize(expr->operator_token.lexeme, expr->operand);
    }

    std::any visit_assign_expr(std::shared_ptr<Assign> expr) override
    {
    }

    std::any visit_logical_expr(std::shared_ptr<Logical> expr) override
    {
    }

    std::any visit_variable_expr(std::shared_ptr<Variable> expr) override
    {
    }

private:
    template <class... E>
    std::string parenthesize(std::string_view name, E... expr)
    {
        assert((... && std::is_same_v<E, std::shared_ptr<Expr>>));

        std::ostringstream builder;

        builder << "(" << name;
        ((builder << " " << print(expr)), ...);
        builder << ")";

        return builder.str();
    }
};
