#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Init the lexer with the input string
Lexer_t *lexer_init(const char *input) {
    Lexer_t *lexer = malloc(sizeof(Lexer_t));
    if (!lexer) {
        fprintf(stderr, "Error: Memory allocation failed for lexer\n");
        return NULL;
    }

    lexer->input = input;
    lexer->pos = 0;
    lexer->length = strlen(input);
    lexer->curr_char = input[0];

    return lexer;
}

// Clean up lexer memory
void lexer_free(Lexer_t *lexer) {
    if (lexer) {
        free(lexer);
    }
}

// Clean up token memory
void token_free(Token_t *token) {
    if (token) {
        if (token->value) {
            free(token->value);
        }
        free(token);
    }
}

// Check if a character is digit
static int is_digit(char c) { return c >= '0' && c <= '9'; }

// Check if a character is any kind of whitespace
static int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

// Advance the current pos by one character
static void advance(Lexer_t *lexer) {
    lexer->pos++;
    if (lexer->pos < lexer->length) {
        lexer->curr_char = lexer->input[lexer->pos];
    } else {
        lexer->curr_char = '\0'; // End of input
    }
}

// Skip over all whitespace characters
static void skip_whitespace(Lexer_t *lexer) {
    while (lexer->curr_char != '\0' && is_whitespace(lexer->curr_char)) {
        advance(lexer);
    }
}

// Read a number token from input
static char *read_number(Lexer_t *lexer) {
    int start_pos = lexer->pos;
    int length = 0;
    int has_decimal = 0;

    // Read consecutive digits
    while (lexer->curr_char != '\0' && is_digit(lexer->curr_char) ||
           lexer->curr_char == '.' && !has_decimal) {

        if (lexer->curr_char == '.') {
            has_decimal = 1;
        }

        advance(lexer);
        length++;
    }

    if (length == 1 && has_decimal) {
        lexer->pos = start_pos;
        lexer->curr_char = lexer->input[lexer->pos];
        return NULL;
    }

    // Allocate memory for the number string
    char *number = malloc(length + 1); // +1 for null terminator
    if (!number) {
        fprintf(stderr, "Error: Memory allocation failed for number\n");
        return NULL;
    }

    // Copy number of characters into new string
    strncpy(number, lexer->input + start_pos, length);
    number[length] = '\0'; // Null terminate the string

    return number;
}

// Create a token with specific type and value
static Token_t *create_token(TokenType type, char *value) {
    Token_t *token = malloc(sizeof(Token_t));
    if (!token) {
        fprintf(stderr, "Error: Memory allocation failed for token\n");
        return NULL;
    }

    token->type = type;
    token->value = value;

    return token;
}

// Create a token for a single character symbol like '+'
static Token_t *create_single_char_token(TokenType type, char ch) {
    char *value = malloc(2); // 1 character + null terminator
    if (!value) {
        fprintf(stderr, "Error: Memory allocation failed for token value\n");
        return NULL;
    }

    value[0] = ch;
    value[1] = '\0';

    return create_token(type, value);
}

// Returns the next token from the input
Token_t *lexer_next_token(Lexer_t *lexer) {
    while (lexer->curr_char != '\0') {
        // Ignore whitespace
        if (is_whitespace(lexer->curr_char)) {
            skip_whitespace(lexer);
            continue;
        }

        // Handle numbers
        if (is_digit(lexer->curr_char)) {
            char *number = read_number(lexer);
            if (number) {
                return create_token(TOKEN_NUMBER, number);
            } else {
                lexer_error(lexer, "Invalid number format");
                return create_single_char_token(TOKEN_ERROR, lexer->curr_char);
            }
        }

        if (lexer->curr_char == '.' && lexer->pos + 1 < lexer->length && is_digit(lexer->input[lexer->pos + 1])) {
            char *number = read_number(lexer);
            if (number) {
                return create_token(TOKEN_NUMBER, number);
            }
        }

        // Handle single character operation
        char ch = lexer->curr_char;
        advance(lexer);

        switch (ch) {
        case '+':
            return create_single_char_token(TOKEN_PLUS, ch);
        case '-':
            return create_single_char_token(TOKEN_MINUS, ch);
        case '*':
            return create_single_char_token(TOKEN_MULTIPLY, ch);
        case '/':
            return create_single_char_token(TOKEN_DIVIDE, ch);
        case '^':
            return create_single_char_token(TOKEN_POWER, ch);
        case '(':
            return create_single_char_token(TOKEN_LPAREN, ch);
        case ')':
            return create_single_char_token(TOKEN_RPAREN, ch);
        default:
            // Unknown character return error token
            lexer_error(lexer, "Unknown character");
            return create_single_char_token(TOKEN_ERROR, ch);
        }
    }

    // Reached end of input
    return create_token(TOKEN_EOF, NULL);
}

// Convert a token type enum to its string name
const char *token_type_to_string(TokenType type) {
    switch (type) {
    case TOKEN_NUMBER:
        return "NUMBER";
    case TOKEN_PLUS:
        return "PLUS";
    case TOKEN_MINUS:
        return "MINUS";
    case TOKEN_MULTIPLY:
        return "MULTIPLY";
    case TOKEN_DIVIDE:
        return "DIVIDE";
    case TOKEN_POWER:
        return "POWER";
    case TOKEN_LPAREN:
        return "LPAREN";
    case TOKEN_RPAREN:
        return "RPAREN";
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

// Print a lexical error message with current character and position
void lexer_error(Lexer_t *lexer, const char *msg) {
    fprintf(stderr, "Lexical Error at position %d: %s '%c' \n", lexer->pos, msg,
            lexer->curr_char);
}

// Debug function to print all the tokens from the input
void print_tokens(const char *input) {
    printf("=== Token Information ===\n");
    printf("Input: %s\n\n", input);

    Lexer_t *lexer = lexer_init(input);
    Token_t *token;

    printf("[ ");
    int first = 1;

    // Keep reading tokens util we hit EOF
    while ((token = lexer_next_token(lexer))->type != TOKEN_EOF) {
        if (!first)
            printf(", ");
        first = 0;

        if (token->value) {
            printf("%s(%s)", token_type_to_string(token->type), token->value);
        } else {
            printf("%s", token_type_to_string(token->type));
        }

        token_free(token);
    }

    printf(" ]\n\n");

    // Free final EOF token and cleanup
    token_free(token);
    lexer_free(lexer);
}
