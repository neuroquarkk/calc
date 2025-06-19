#include "../include/lexer.h"
#include "../include/parser.h"
#include <stdio.h>
#include <string.h>

// Function to demonstrate lexer functionality
void demo_lexer(const char *input) {
    printf("=== LEXER DEMO ===\n");
    print_tokens(input);
}

// Function to demonstrate parser and evaluation
void demo_parser(const char *input) {
    printf("=== PARSER DEMO ===\n");
    printf("Input: %s\n\n", input);

    // Init lexer
    Lexer_t *lexer = lexer_init(input);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to initialize lexer\n");
        return;
    }

    // Init parser
    Parser_t *parser = parser_init(lexer);
    if (!parser) {
        fprintf(stderr, "Error: Failed to initialize parser\n");
        lexer_free(lexer);
        return;
    }

    // Parse the expression into AST
    ASTNode_t *ast = parser_parse(parser);
    if (ast) {
        // Print AST structure
        printf("AST Structure\n");
        ast_print(ast, 0);
        printf("\n");

        double result = ast_eval(ast);
        printf("Result: %.6g\n", result);

        ast_free(ast);
    } else {
        fprintf(stderr, "Error: Faild to parse expression\n");
    }

    parser_free(parser);
    lexer_free(lexer);
    printf("\n");
}

// Interactive mode
void interactive_mode() {
    char input[256];

    printf("=== INTERACTIVE CALCULATOR ===\n");
    printf("Enter arithmetic expressions ('quit' to exit): \n");
    printf("Supported operators: +, -, *, /, (, )\n");

    while (1) {
        printf("calc> ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin))
            break;

        // Skip empty line
        if (strlen(input) == 0)
            continue;

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Check for quick command
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0)
            break;

        // Processes the expression
        Lexer_t *lexer = lexer_init(input);
        if (!lexer) {
            fprintf(stderr, "Error: Failed to initialize lexer\n");
            return;
        }

        Parser_t *parser = parser_init(lexer);
        if (!parser) {
            fprintf(stderr, "Error: Failed to initialize parser\n");
            lexer_free(lexer);
            return;
        }

        ASTNode_t *ast = parser_parse(parser);
        if (ast) {
            double result = ast_eval(ast);
            printf("= %0.6g\n", result);
            ast_free(ast);
        }

        parser_free(parser);
        lexer_free(lexer);
    }

    printf("Goodbye\n");
}

// Function to run predefined test cases
void run_tests() {
    printf("=== RUNNING TEST CASE ===\n\n");

    const char *test_cases[] = {
        "42",                    // Simple number
        "3 + 4",                 // Basic addition
        "10 - 3 * 2",            // Precedence test
        "(5 + 3) * 2",           // Parentheses
        "2 ^ 3 ^ 2",             // Right-associative power
        "-5 + 3",                // Unary minus
        "+(4 * 3)",              // Unary plus
        "100 / 4 / 5",           // Left-associative division
        "3 + 4 * 2^2 - (5 + 1)", // Complex expression
        "2 * (3 + 4) ^ 2 / 7",   // Another complex one
    };

    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_tests; i++) {
        printf("Test %d: %s\n", i + 1, test_cases[i]);

        Lexer_t *lexer = lexer_init(test_cases[i]);
        Parser_t *parser = parser_init(lexer);
        ASTNode_t *ast = parser_parse(parser);

        if (ast) {
            double result = ast_eval(ast);
            printf("Result: %.6g\n", result);
            ast_free(ast);
        } else {
            printf("Parse failed\n");
        }

        parser_free(parser);
        lexer_free(lexer);
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    printf("Arithmetic Expression Compiler\n");
    printf("==============================\n");

    // If no argument run in interactive mode
    if (argc == 1) {
        interactive_mode();
        return 0;
    }

    char *command = argv[1];

    // Handle command line arguments
    if (argc == 2) {
        if (strcmp(command, "--test") == 0 || strcmp(command, "-t") == 0) {
            run_tests();
            return 0;
        } else if (strcmp(command, "--help") == 0 || strcmp(command, "-h") == 0) {
            printf("Usage:\n");
            printf("  calc                    - Interactive calculator mode\n");
            printf("  calc \"expression\"       - Evaluate single expression\n");
            printf("  calc --test             - Run test cases\n");
            printf("  calc --demo \"expr\"      - Show lexer and parser demo\n");
            printf("  calc --help             - Show this help\n");
            return 0;
        } else {
            // Treat as expression to evaluate
            const char *expression = command;

            Lexer_t *lexer = lexer_init(expression);
            Parser_t *parser = parser_init(lexer);
            ASTNode_t *ast = parser_parse(parser);

            if (!ast) {
                fprintf(stderr, "Error: Failed to parser expression: %s\n", expression);
                return 1;
            }

            double result = ast_eval(ast);
            printf("Input: %s\n", expression);
            printf("Result: %.6g\n", result);
            ast_free(ast);

            parser_free(parser);
            return 0;
        }
    }

    // Handle --demo option
    if (argc == 3 && strcmp(command, "--demo") == 0) {
        const char *expression = argv[2];
        demo_lexer(expression);
        demo_parser(expression);
        return 0;
    }

    printf("Error: Invalid arguments. Use --help for usage information.\n");
    return 1;
}
