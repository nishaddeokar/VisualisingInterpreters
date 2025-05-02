#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "token.h"

/**
 * Token Printer - Creates colourised terminal output for token streams
 * Shows source code with syntax highlighting and token details
 */
class TokenPrinter
{
private:
    // ANSI colour codes
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string KEYWORD_COLOUR = "\033[34m";     // Blue for keywords
    const std::string IDENTIFIER_COLOUR = "\033[32m";  // Green for identifiers
    const std::string LITERAL_COLOUR = "\033[33m";     // Yellow for literals
    const std::string OPERATOR_COLOUR = "\033[35m";    // Purple for operators
    const std::string DELIMITER_COLOUR = "\033[31m";   // Red/Orange for delimiters
    const std::string LINE_NUMBER_COLOUR = "\033[36m"; // Cyan for line numbers

    // Get colour for a token type
    std::string get_token_colour(TokenType type) const
    {
        // Keywords
        if (type >= AND && type <= WHILE)
            return KEYWORD_COLOUR;

        // Identifiers
        if (type == IDENTIFIER)
            return IDENTIFIER_COLOUR;

        // Literals
        if (type == STRING || type == NUMBER || type == TRUE || type == FALSE || type == NIL)
            return LITERAL_COLOUR;

        // Operators
        if ((type >= BANG && type <= LESS_EQUAL) ||
            type == MINUS || type == PLUS || type == SLASH || type == STAR)
            return OPERATOR_COLOUR;

        // Delimiters
        if (type <= RIGHT_BRACE || type == COMMA || type == DOT || type == SEMICOLON)
            return DELIMITER_COLOUR;

        return RESET;
    }

public:
    /**
     * Display the source code with syntax highlighting
     */
    void print_colourised_source(std::string_view source, const std::vector<Token> &tokens)
    {
        std::cout << BOLD << "\nSOURCE CODE WITH HIGHLIGHTING" << RESET << "\n\n";

        // Create a map of positions to tokens for colouring
        std::vector<std::pair<size_t, const Token *>> token_positions;

        // Find each token's position in the source
        size_t pos = 0;
        for (const Token &token : tokens)
        {
            // Skip EOF token
            if (token.type == END_OF_FILE)
                continue;

            // Find token position in source
            size_t token_pos = source.find(token.lexeme, pos);
            if (token_pos != std::string::npos)
            {
                token_positions.push_back({token_pos, &token});
                pos = token_pos + token.lexeme.length();
            }
        }

        // Sort positions
        std::sort(token_positions.begin(), token_positions.end(),
                  [](const auto &a, const auto &b)
                  { return a.first < b.first; });

        // Display source with colours
        size_t current_pos = 0;
        int line_number = 1;

        std::cout << LINE_NUMBER_COLOUR << std::setw(4) << line_number << " |" << RESET << " ";

        for (const auto &[pos, token_ptr] : token_positions)
        {
            // Print any text before the token (whitespace, comments)
            if (pos > current_pos)
            {
                std::string_view before = source.substr(current_pos, pos - current_pos);

                // Handle line breaks in the "before" text
                for (char c : before)
                {
                    std::cout << c;
                    if (c == '\n')
                    {
                        line_number++;
                        std::cout << LINE_NUMBER_COLOUR << std::setw(4) << line_number << " |" << RESET << " ";
                    }
                }
            }

            // Get colour for this token
            std::string colour = get_token_colour(token_ptr->type);

            // Print the token with colour
            std::string token_text = std::string(token_ptr->lexeme);
            std::cout << colour << token_text << RESET;

            // Update position
            current_pos = pos + token_text.length();
        }

        // Print anything remaining
        if (current_pos < source.length())
        {
            std::string_view remainder = source.substr(current_pos);
            for (char c : remainder)
            {
                std::cout << c;
                if (c == '\n')
                {
                    line_number++;
                    std::cout << LINE_NUMBER_COLOUR << std::setw(4) << line_number << " |" << RESET << " ";
                }
            }
        }

        std::cout << '\n'
                  << std::endl;
    }

    /**
     * Print detailed token information
     */
    void print_token_list(const std::vector<Token> &tokens)
    {
        std::cout << BOLD << "TOKEN LIST" << RESET << "\n\n";

        // Group tokens by line number
        std::map<int, std::vector<const Token *>> tokens_by_line;
        for (const Token &token : tokens)
        {
            tokens_by_line[token.line_number].push_back(&token);
        }

        // Print header
        std::cout << std::left
                  << std::setw(5) << "IDX"
                  << std::setw(20) << "TYPE"
                  << std::setw(30) << "LEXEME"
                  << "LINE" << std::endl;

        std::cout << std::string(65, '-') << std::endl;

        // Print tokens grouped by line
        for (const auto &[line_number, line_tokens] : tokens_by_line)
        {
            // Print line separator if not the first line
            if (line_number > 1)
            {
                std::cout << std::string(65, '-') << std::endl;
            }

            // Print each token in this line with index starting from 0
            for (size_t i = 0; i < line_tokens.size(); i++)
            {
                const Token *token = line_tokens[i];
                std::string colour = get_token_colour(token->type);

                std::cout << std::setw(5) << i
                          << colour << std::setw(20) << ::to_string(token->type)
                          << std::setw(30) << token->lexeme << RESET
                          << line_number << std::endl;
            }
        }

        std::cout << std::endl;
    }

    /**
     * Visualise tokens with both colourised source and token details
     */
    void visualise_tokens(std::string_view source, const std::vector<Token> &tokens)
    {
        print_colourised_source(source, tokens);
        print_token_list(tokens);
    }
};
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "token.h"

/**
 * Token Printer - Creates colourised terminal output for token streams
 * Shows source code with syntax highlighting and token details
 */
class TokenPrinter
{
private:
    // ANSI colour codes
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string KEYWORD_COLOUR = "\033[34m";     // Blue for keywords
    const std::string IDENTIFIER_COLOUR = "\033[32m";  // Green for identifiers
    const std::string LITERAL_COLOUR = "\033[33m";     // Yellow for literals
    const std::string OPERATOR_COLOUR = "\033[35m";    // Purple for operators
    const std::string DELIMITER_COLOUR = "\033[31m";   // Red/Orange for delimiters
    const std::string LINE_NUMBER_COLOUR = "\033[36m"; // Cyan for line numbers

    // Get colour for a token type
    std::string get_token_colour(TokenType type) const
    {
        // Keywords
        if (type >= AND && type <= WHILE)
            return KEYWORD_COLOUR;

        // Identifiers
        if (type == IDENTIFIER)
            return IDENTIFIER_COLOUR;

        // Literals
        if (type == STRING || type == NUMBER || type == TRUE || type == FALSE || type == NIL)
            return LITERAL_COLOUR;

        // Operators
        if ((type >= BANG && type <= LESS_EQUAL) ||
            type == MINUS || type == PLUS || type == SLASH || type == STAR)
            return OPERATOR_COLOUR;

        // Delimiters
        if (type <= RIGHT_BRACE || type == COMMA || type == DOT || type == SEMICOLON)
            return DELIMITER_COLOUR;

        return RESET;
    }

public:
    /**
     * Display the source code with syntax highlighting
     */
    void print_colourised_source(std::string_view source, const std::vector<Token> &tokens)
    {
        std::cout << BOLD << "\n=== SOURCE CODE WITH HIGHLIGHTING ===" << RESET << "\n\n";

        // Create a map of positions to tokens for colouring
        std::vector<std::pair<size_t, const Token *>> token_positions;

        // Find each token's position in the source
        size_t pos = 0;
        for (const Token &token : tokens)
        {
            // Skip EOF token
            if (token.type == END_OF_FILE)
                continue;

            // Find token position in source
            size_t token_pos = source.find(token.lexeme, pos);
            if (token_pos != std::string::npos)
            {
                token_positions.push_back({token_pos, &token});
                pos = token_pos + token.lexeme.length();
            }
        }

        // Sort positions
        std::sort(token_positions.begin(), token_positions.end(),
                  [](const auto &a, const auto &b)
                  { return a.first < b.first; });

        // Display source with colours
        size_t current_pos = 0;
        int line_number = 1;

        std::cout << LINE_NUMBER_COLOUR << std::setw(4) << line_number << " |" << RESET << " ";

        for (const auto &[pos, token_ptr] : token_positions)
        {
            // Print any text before the token (whitespace, comments)
            if (pos > current_pos)
            {
                std::string_view before = source.substr(current_pos, pos - current_pos);

                // Handle line breaks in the "before" text
                for (char c : before)
                {
                    std::cout << c;
                    if (c == '\n')
                    {
                        line_number++;
                        std::cout << LINE_NUMBER_COLOUR << std::setw(4) << line_number << " |" << RESET << " ";
                    }
                }
            }

            // Get colour for this token
            std::string colour = get_token_colour(token_ptr->type);

            // Print the token with colour
            std::string token_text = std::string(token_ptr->lexeme);
            std::cout << colour << token_text << RESET;

            // Update position
            current_pos = pos + token_text.length();
        }

        // Print anything remaining
        if (current_pos < source.length())
        {
            std::string_view remainder = source.substr(current_pos);
            for (char c : remainder)
            {
                std::cout << c;
                if (c == '\n')
                {
                    line_number++;
                    std::cout << LINE_NUMBER_COLOUR << std::setw(4) << line_number << " |" << RESET << " ";
                }
            }
        }

        std::cout << '\n'
                  << std::endl;
    }

    /**
     * Print detailed token information
     */
    void print_token_list(const std::vector<Token> &tokens)
    {
        std::cout << BOLD << "=== TOKEN LIST ===" << RESET << "\n\n";

        // Group tokens by line number
        std::map<int, std::vector<const Token *>> tokens_by_line;
        for (const Token &token : tokens)
        {
            tokens_by_line[token.line_number].push_back(&token);
        }

        // Print header
        std::cout << std::left
                  << std::setw(5) << "IDX"
                  << std::setw(20) << "TYPE"
                  << std::setw(30) << "LEXEME"
                  << "LINE" << std::endl;

        std::cout << std::string(65, '-') << std::endl;

        // Print tokens grouped by line
        for (const auto &[line_number, line_tokens] : tokens_by_line)
        {
            // Print line separator if not the first line
            if (line_number > 1)
            {
                std::cout << std::string(65, '-') << std::endl;
            }

            // Print each token in this line with index starting from 0
            for (size_t i = 0; i < line_tokens.size(); i++)
            {
                const Token *token = line_tokens[i];
                std::string colour = get_token_colour(token->type);

                std::cout << std::setw(5) << i
                          << colour << std::setw(20) << ::to_string(token->type)
                          << std::setw(30) << token->lexeme << RESET
                          << line_number << std::endl;
            }
        }

        std::cout << std::endl;
    }

    /**
     * Visualise tokens with both colourised source and token details
     */
    void visualise_tokens(std::string_view source, const std::vector<Token> &tokens)
    {
        print_colourised_source(source, tokens);
        print_token_list(tokens);
    }
};
