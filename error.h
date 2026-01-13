#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include "runtime_error.h"
#include "token.h"

// Global error state flags
inline bool had_error = false;
inline bool had_runtime_error = false;

/**
 * Logs error information to standard error output
 *
 * @param line_num The line where the error occurred
 * @param context Additional context about error location
 * @param error_msg The error message to display
 */
static void report(int line_num, std::string_view context,
                   std::string_view error_msg)
{
    // Build error message
    std::ostringstream error_output;
    error_output << "[line " << line_num << "] Error"
                 << context << ": " << error_msg << "\n";

    // Output to stderr
    std::cerr << error_output.str();

    // Set the error flag
    had_error = true;
}

/**
 * Reports a syntax error at a specific token
 *
 * @param token The token where the error occurred
 * @param error_msg Description of the error
 */
void error(const Token &token, std::string_view error_msg)
{
    std::string context;

    if (token.type == END_OF_FILE)
    {
        // Error at the end of file
        context = " at end";
    }
    else
    {
        // Error at a specific token
        context = " at '" + token.lexeme + "'";
    }

    // Report the error with context
    report(token.line_number, context, error_msg);
}

/**
 * Reports a syntax error at a specific line
 *
 * @param line_num The line where the error occurred
 * @param error_msg Description of the error
 */
void error(int line_num, std::string_view error_msg)
{
    // Report the error with no additional context
    report(line_num, "", error_msg);
}

/**
 * Reports a runtime error with information from the exception
 *
 * @param runtime_exception The runtime error that occurred
 */
void runtime_error(const RuntimeError &runtime_exception)
{
    // Construct and output the error message with line information
    std::ostringstream error_output;
    error_output << runtime_exception.what() << "\n"
                 << "[line " << runtime_exception.token.line_number << "]\n";

    std::cerr << error_output.str();

    // Set the runtime error flag
    had_runtime_error = true;
}
