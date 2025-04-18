#pragma once

#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <cctype>
#include "Error.h"
#include "Token.h"

// Lexical analyzer for tokenizing source code
class Lexer
{
private:
    // Dictionary of reserved keywords
    static const std::map<std::string, TokenType> keywords;

    // Source text and parsing state
    std::string_view source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    // Character classification helpers
    bool is_end() const
    {
        return current >= input.size();
    }

    bool is_alpha(char c) const
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    bool is_digit(char c) const
    {
        return c >= '0' && c <= '9';
    }

    bool is_alphanumeric(char c) const
    {
        return is_alpha(c) || is_digit(c);
    }

    // Input navigation
    char advance()
    {
        return input[current++];
    }

    char peek() const
    {
        return is_end() ? '\0' : input[current];
    }

    char peek_next() const
    {
        return (current + 1 >= input.size()) ? '\0' : input[current + 1];
    }

    bool match(char expected)
    {
        if (is_end() || input[current] != expected)
            return false;
        current++;
        return true;
    }

    // Token creation
    void emit_token(TokenType type)
    {
        emit_token(type, nullptr);
    }

    void emit_token(TokenType type, std::any literal)
    {
        std::string lexeme{input.substr(start, current - start)};
        output.emplace_back(type, std::move(lexeme), std::move(literal), line);
    }

    // Token processing methods
    void process_identifier()
    {
        // Consume the entire identifier
        while (is_alphanumeric(peek()))
            advance();

        // Check if it's a keyword or user identifier
        std::string text{input.substr(start, current - start)};
        auto keyword_iter = reserved_words.find(text);

        if (keyword_iter != reserved_words.end())
        {
            emit_token(keyword_iter->second); // It's a keyword
        }
        else
        {
            emit_token(IDENTIFIER); // It's a user-defined identifier
        }
    }

    void process_number()
    {
        // Parse integer part
        while (is_digit(peek()))
            advance();

        // Check for decimal part
        if (peek() == '.' && is_digit(peek_next()))
        {
            advance(); // Consume the decimal point

            // Parse fractional part
            while (is_digit(peek()))
                advance();
        }

        // Convert to numeric value
        std::string num_str{input.substr(start, current - start)};
        emit_token(NUMBER, std::stod(num_str));
    }

    void process_string()
    {
        // Find the closing quote
        while (peek() != '"' && !is_end())
        {
            if (peek() == '\n')
                line++;
            advance();
        }

        // Check for unterminated string
        if (is_end())
        {
            error(line, "Unterminated string.");
            return;
        }

        // Consume the closing quote
        advance();

        // Extract the string value (without quotes)
        std::string value{input.substr(start + 1, current - start - 2)};
        emit_token(STRING, value);
    }

    void process_token()
    {
        char c = advance();

        // Process based on character type
        if (c == '(')
            emit_token(LEFT_PAREN);
        else if (c == ')')
            emit_token(RIGHT_PAREN);
        else if (c == '{')
            emit_token(LEFT_BRACE);
        else if (c == '}')
            emit_token(RIGHT_BRACE);
        else if (c == ',')
            emit_token(COMMA);
        else if (c == '.')
            emit_token(DOT);
        else if (c == '-')
            emit_token(MINUS);
        else if (c == '+')
            emit_token(PLUS);
        else if (c == ';')
            emit_token(SEMICOLON);
        else if (c == '*')
            emit_token(STAR);

        // Two-character operators
        else if (c == '!')
            emit_token(match('=') ? BANG_EQUAL : BANG);
        else if (c == '=')
            emit_token(match('=') ? EQUAL_EQUAL : EQUAL);
        else if (c == '<')
            emit_token(match('=') ? LESS_EQUAL : LESS);
        else if (c == '>')
            emit_token(match('=') ? GREATER_EQUAL : GREATER);

        // Special handling for slash (division or comment)
        else if (c == '/')
        {
            if (match('/'))
            {
                // Skip comments until end of line
                while (peek() != '\n' && !is_end())
                    advance();
            }
            else
            {
                emit_token(SLASH);
            }
        }

        // Whitespace handling
        else if (c == ' ' || c == '\r' || c == '\t')
        {
            // Ignore whitespace
        }

        // Line breaks
        else if (c == '\n')
        {
            line++;
        }

        // String literals
        else if (c == '"')
        {
            process_string();
        }

        // Numbers, identifiers, or errors
        else if (is_digit(c))
        {
            process_number();
        }
        else if (is_alpha(c))
        {
            process_identifier();
        }
        else
        {
            error(line, "Unexpected character.");
        }
    }

public:
    // Constructor takes source code to analyze
    Scanner(std::string_view source) : input(source) {}

    // Main method to tokenize the source code
    std::vector<Token> scanTokens()
    {
        // Continue until end of input
        while (!is_end())
        {
            // Mark start of current token
            start = current;
            process_token();
        }

        // Add end-of-file token
        output.emplace_back(END_OF_FILE, "", nullptr, line);
        return output;
    }
};

// Initialize the reserved keywords lookup table
const std::map<std::string, TokenType> Scanner::reserved_words = {
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},
    {"for", TokenType::FOR},
    {"fun", TokenType::FUN},
    {"if", TokenType::IF},
    {"nil", TokenType::NIL},
    {"or", TokenType::OR},
    {"print", TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super", TokenType::SUPER},
    {"this", TokenType::THIS},
    {"true", TokenType::TRUE},
    {"var", TokenType::VAR},
    {"while", TokenType::WHILE},
};