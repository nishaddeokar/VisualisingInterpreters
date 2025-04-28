#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include "ast_printer.h"
#include "token_printer.h"
#include "error.h"
#include "interpreter.h"
#include "parser.h"
#include "lexer.h"

// Environment state
Interpreter interpreter{};

// Visualization mode flags
bool visual_mode = false;
bool token_mode = false; // New flag for token visualization

// File operations
std::string read_file(std::string_view filename)
{
    // Open the file at the end to get size
    std::ifstream input_stream{
        filename.data(),
        std::ios::binary | std::ios::in | std::ios::ate};

    // Check for errors
    if (!input_stream)
    {
        std::cerr << "Could not open file '" << filename
                  << "': " << std::strerror(errno) << "\n";
        std::exit(74); // IO error code
    }

    // Prepare buffer with correct size
    auto file_size = input_stream.tellg();
    std::string buffer;
    buffer.resize(file_size);

    // Read the whole file
    input_stream.seekg(0);
    input_stream.read(&buffer[0], file_size);

    return buffer;
}

// Execution functions
void run(std::string_view code, bool is_interactive = false)
{
    // Step 1: Lexical analysis
    Lexer lexer{code};
    std::vector<Token> tokens = lexer.scan_tokens();

    // Step 1.5: Token visualization if in token mode
    if (token_mode)
    {
        TokenPrinter token_printer;
        token_printer.visualize_tokens(code, tokens);
    }

    // Step 2: Syntax analysis
    Parser parser{tokens};
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    // Stop if syntax errors were found
    if (had_error)
    {
        return;
    }

    // Step 3: Visualization if in visual mode
    if (visual_mode)
    {
        AstPrinter printer;
        if (is_interactive)
        {
            // For interactive mode, visualize each statement separately
            // and reuse the same output file
            if (!statements.empty())
            {
                printer.visualize_stmt(statements.back(), "ast");
            }
        }
        else
        {
            // For file execution, visualize the whole program
            printer.visualize_program(statements, "program_ast");
        }
    }

    // Step 4: Execution
    interpreter.interpret(statements);
}

void execute_file(std::string_view path)
{
    // Load and execute the file
    auto source = read_file(path);
    run(source, false); // Not interactive

    // Handle errors with appropriate exit codes
    if (had_error)
    {
        std::exit(65); // Syntax error
    }

    if (had_runtime_error)
    {
        std::exit(70); // Runtime error
    }
}

void interactive_shell()
{
    std::string input_line;

    std::cout << "Prism";
    if (visual_mode)
    {
        std::cout << " (Visual Mode)";
    }
    if (token_mode)
    {
        std::cout << " (Token Mode)";
    }
    std::cout << "\n";

    while (true)
    {
        // Display prompt
        std::cout << "> ";

        // Get user input
        bool has_input = static_cast<bool>(std::getline(std::cin, input_line));

        // Exit on EOF
        if (!has_input)
        {
            break;
        }

        // Execute the entered code
        run(input_line, true); // Interactive mode

        // Reset error state for next line
        had_error = false;
    }
}

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        // Handle visual mode flag
        if (std::string(argv[i]) == "-v" || std::string(argv[i]) == "--visual")
        {
            visual_mode = true;
            // Shift remaining arguments left
            for (int j = i; j < argc - 1; j++)
            {
                argv[j] = argv[j + 1];
            }
            argc--; // One less argument to process
            i--;    // Process the current position again
            continue;
        }

        // Handle token mode flag
        if (std::string(argv[i]) == "-t" || std::string(argv[i]) == "--token")
        {
            token_mode = true;
            // Shift remaining arguments left
            for (int j = i; j < argc - 1; j++)
            {
                argv[j] = argv[j + 1];
            }
            argc--; // One less argument to process
            i--;    // Process the current position again
            continue;
        }
    }

    if (argc > 2)
    {
        std::cout << "Usage: prism [-v] [-t] [script]\n";
        std::exit(64);
    }
    else if (argc == 2)
    {
        execute_file(argv[1]);
    }
    else
    {
        interactive_shell();
    }
}
