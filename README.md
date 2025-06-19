# calc
A complete arithmetic expression compiler build in C that lexically analyzes, parses and evaluates mathematical expressions with proper operator precedence and associativity.

## Features
- **Supported Operations**
    - **Arithmetic operators:** `+`, `-`, `*`, `/`, `^`(power)
    - **Parantheses:** `(`, `)` for grouping expressions
    - **Unary operators:** `+`, `-` (positive/negative numbers)

- **Operator Precedence (highest to lowest)**
    1. Parantheses `( )`
    2. Unary operators `+`, `-`
    3. Power `^` (right associative)
    4. Multiplication/Division `*`, `/` (left associative)
    5. Addition/Subtraction `+`, `-` (left associative)

## Quick Start
### **Prerequisites**
 - GCC compiler
 - Make utility
 - Standard C libary with math support

### **Installation**
```bash
git clone https://github.com/neuroquarkk/calc
cd calc

make

./bin/calc
```

## Usage
|  Mode                   |  Command Format                      |  Example                           |
| ----------------------- | ---------------------------------- | ------------------------------------ |
| **Interactive Mode**    | `./bin/calc`                       | `./bin/calc`                         |
| **Evaluate Expression** | `./bin/calc "<expression>"`        | `./bin/calc "3 + 4 * 2^2 - (5 + 1)"` |
| **Run Test Cases**      | `./bin/calc --test` or `-t`        | `./bin/calc --test`                  |
| **Demo Lexer & Parser** | `./bin/calc --demo "<expression>"` | `./bin/calc --demo "3 + 4 * 2"`      |
| **Help Information**    | `./bin/calc --help` or `-h`        | `./bin/calc --help`                  |


## Project Structure
<pre>
calc/
├── README.md
├── Makefile
├── main.c                  # Main program with multiple modes
├── include/
│   ├── lexer.h            # Lexer interface
│   └── parser.h           # Parser and AST interface
├── src/
│   ├── lexer.c            # Lexical analyzer implementation
│   └── parser.c           # Parser and evaluator implementation
├── build/                 # Object files (auto-generated)
│   ├── lexer.o
│   ├── parser.o
│   └── main.o
└── bin/
    └── calc               # Final compiled binary
</pre>

## Make Options
```bash
make            # Default builds the project in release mode (optimized)
make release    # Builds the eproject with optimization flag (-O2)
make debug      # Builds the project with debug symbols and warnings (-g -Wall -DDEBUG)
make run        # Runs the compiled binary (bin/calc) with rebuilding
make clean      # Removes build/ directories
make distclean  # Full cleanup including bin/
make help       # Show make help message
```

## How It Works
- **Lexer:** Converts raw input into tokens
- **Parser:** Builds an Abstract Syntax Tree (AST) based on operator precedence
- **Evaluator:** Recursively computes the AST to get the final result
