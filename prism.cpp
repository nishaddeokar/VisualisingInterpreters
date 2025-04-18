#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "error.h"
// #include "interpreter.h"
// #include "parser.h"
#include "lexer.h"

// Environment state
// Interpreter interpreter{};

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
void execute_source(std::string_view code)
{
    // Step 1: Lexical analysis
    Lexer lexer{code};
    std::vector<Token> tokens = lexer.scan_tokens();

    // Print the tokens.
    for (const Token &token : tokens)
    {
        std::cout << token.to_string() << "\n";
    }

    // // Step 2: Syntax analysis
    // Parser syntax_analyzer{token_stream};
    // auto ast = syntax_analyzer.parse();

    // // Stop if syntax errors were found
    // if (hadError)
    // {
    //     return;
    // }

    // // Handle potential resolution issues
    // if (hadError)
    // {
    //     return;
    // }

    // // Step 3: Execution
    // interpreter.interpret(ast);
}

void execute_file(std::string_view path)
{
    // Load and execute the file
    auto source = read_file(path);
    execute_source(source);

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
        execute_source(input_line);

        // Reset error state for next line
        had_error = false;
    }
}

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        std::cout << "Usage: prism [script]\n";
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
