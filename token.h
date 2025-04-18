#pragma once

#include <any>
#include <string>
#include <utility>
#include <sstream>
#include "token_type.h"

/**
 * Represents a lexical token from the source code.
 * Each token contains information about its type, text,
 * associated value, and source location.
 */
class Token
{
private:
    // Helper method to extract the literal value as string
    std::string get_literal_string() const
    {
        // Convert the literal value to a string based on token type
        if (type == IDENTIFIER)
        {
            return lexeme;
        }
        else if (type == STRING)
        {
            return std::any_cast<std::string>(value);
        }
        else if (type == NUMBER)
        {
            return std::to_string(std::any_cast<double>(value));
        }
        else if (type == TRUE)
        {
            return "true";
        }
        else if (type == FALSE)
        {
            return "false";
        }
        else
        {
            return "nil";
        }
    }

public:
    // Token properties - all immutable
    const TokenType type;
    const std::string lexeme;
    const std::any value;
    const int line_number;

    // Constructor to initialize all fields
    Token(TokenType token_type, std::string text, std::any literal_value, int source_line)
        : type(token_type),
          lexeme(std::move(text)),
          value(std::move(literal_value)),
          line_number(source_line)
    {
    }

    // Generates a string representation of this token
    std::string to_string() const
    {
        // Format: TokenType text literal_value
        std::ostringstream output;
        output << ::to_string(type) << " "
               << lexeme << " "
               << get_literal_string();
        return output.str();
    }
};
