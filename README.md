# Visualising Interpreters

## Basic Usage

Run the program by opening the terminal in the prism_app folder and entering the command:

./prism [options] [script]

Command-line Options
No arguments: Run in interactive shell mode
-v or --visual: Enable AST visualisation mode (generates graphical AST diagrams)
-t or --token: Enable token visualisation mode (displays colourised tokens)

## Execution Modes

### Interactive Shell
Run the interpreter without a script file to enter interactive mode:

In interactive mode, you can type Prism code line by line (statement by statement) at the prompt (>).

### Script Execution
Run a Prism script file:

./prism script.prism

## Visualisation Modes

### Token Mode
Run any example with the -t flag to visualise tokens:

./prism -t scope.prism

This displays:
Source code with syntax highlighting
A detailed token list showing each token's type, lexeme, and line number

### Visual Mode (AST)

Run any example with the -v flag to generate abstract syntax tree visualisation:

./prism -v fibonacci.prism

This creates a PNG file in the images directory showing the program's abstract syntax tree structure.

### Combined Mode

You can combine both visualisation modes!

./prism -t -v fibonacci.prism
