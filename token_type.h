#pragma once

#include <string>
#include <array>

/**
 * All possible token types in the language.
 * Organised by categories for better readability.
 */
enum TokenType
{
    // Grouping delimiters
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,

    // Separators and operators
    COMMA,
    DOT,
    SEMICOLON,
    MINUS,
    PLUS,
    SLASH,
    STAR,

    // Comparison operators
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literal values
    IDENTIFIER,
    STRING,
    NUMBER,

    // Language keywords
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    // Special token
    END_OF_FILE
};

/**
 * Converts a token type to its string representation.
 *
 * @param type The token type to convert
 * @return A string representation of the token type
 */
std::string to_string(TokenType type)
{
    // Define all token type names
    constexpr int TOKEN_COUNT = 40; // Total number of token types

    // Create an array of token names indexed by their enum value
    static const std::array<std::string, TOKEN_COUNT> token_names = {
        // Grouping delimiters
        "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",

        // Separators and operators
        "COMMA", "DOT", "SEMICOLON",
        "MINUS", "PLUS", "SLASH", "STAR",

        // Comparison operators
        "BANG", "BANG_EQUAL",
        "EQUAL", "EQUAL_EQUAL",
        "GREATER", "GREATER_EQUAL",
        "LESS", "LESS_EQUAL",

        // Literal values
        "IDENTIFIER", "STRING", "NUMBER",

        // Language keywords
        "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
        "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",

        // Special token
        "END_OF_FILE"};

    // Convert enum value to array index and return corresponding name
    return token_names[static_cast<int>(type)];
}
