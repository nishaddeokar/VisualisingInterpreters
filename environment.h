#pragma once

#include <any>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include "error.h"
#include "token.h"
#include "runtime_error.h"

/**
 * Environment for storing and accessing variable values
 * Implements lexical scoping with nested environments
 */
class Environment : public std::enable_shared_from_this<Environment>
{
private:
    // Storage for variable definitions
    std::map<std::string, std::any> variable_store;

    // Parent environment for nested scopes
    std::shared_ptr<Environment> parent_scope;

    // Allow Interpreter to access private members
    friend class Interpreter;

public:
    /**
     * Creates a global environment with no parent
     */
    Environment()
        : parent_scope{nullptr}
    {
    }

    /**
     * Creates a local environment with specified parent
     * @param parent The enclosing environment
     */
    Environment(std::shared_ptr<Environment> parent)
        : parent_scope{std::move(parent)}
    {
    }

    /**
     * Retrieves a variable's value from the environment
     * @param name_token Token containing the variable name
     * @return The variable's value
     * @throws RuntimeError if variable doesn't exist
     */
    std::any get(const Token &name_token)
    {
        // Try to find in current environment
        std::string var_name = name_token.lexeme;
        auto iter = variable_store.find(var_name);

        if (iter != variable_store.end())
        {
            // Found in current scope
            return iter->second;
        }

        // Check parent scope if it exists
        if (parent_scope != nullptr)
        {
            return parent_scope->get(name_token);
        }

        // Variable not found in any scope
        throw RuntimeError(name_token,
                           "Undefined variable '" + var_name + "'.");
    }

    /**
     * Updates an existing variable's value
     * @param name_token Token containing the variable name
     * @param new_value The new value to assign
     * @throws RuntimeError if variable doesn't exist
     */
    void assign(const Token &name_token, std::any new_value)
    {
        // Try to assign in current environment
        std::string var_name = name_token.lexeme;
        auto iter = variable_store.find(var_name);

        if (iter != variable_store.end())
        {
            // Update in current scope
            iter->second = std::move(new_value);
            return;
        }

        // Try to assign in parent scope
        if (parent_scope != nullptr)
        {
            parent_scope->assign(name_token, std::move(new_value));
            return;
        }

        // Variable not found in any scope
        throw RuntimeError(name_token,
                           "Cannot assign to undefined variable '" + var_name + "'.");
    }

    /**
     * Creates or updates a variable in the current scope
     * @param var_name The variable name as a string
     * @param init_value The initial value to assign
     */
    void define(const std::string &var_name, std::any init_value)
    {
        // Add or replace in current scope only
        variable_store[var_name] = std::move(init_value);
    }
};
