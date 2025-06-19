#ifndef LEXER_H
#define LEXER_H

// Token types recongnized by the lexer
typedef enum {
    TOKEN_NUMBER,   // "123", "1"
    TOKEN_PLUS,     // +
    TOKEN_MINUS,    // -
    TOKEN_MULTIPLY, // *
    TOKEN_DIVIDE,   // /
    TOKEN_POWER,    // ^
    TOKEN_LPAREN,   // (
    TOKEN_RPAREN,   // )
    TOKEN_EOF,      // end of input (no more tokens)
    TOKEN_ERROR,    // unrecongnized or invalid character
} TokenType;

// Represents a single token in the input
typedef struct {
    TokenType type; // Kind of token
    char *value;    // Actual text of the token
} Token_t;

// Represents the state of the lexer while processing input
typedef struct {
    const char *input; // full input string
    int pos;           // current position index
    int length;        // total length of the input
    int curr_char;     // current character being processed
} Lexer_t;

Lexer_t *lexer_init(const char *input);
void lexer_free(Lexer_t *lexer);
void token_free(Token_t *token);
Token_t *lexer_next_token(Lexer_t *lexer);
const char *token_type_to_string(TokenType type);
void lexer_error(Lexer_t *lexer, const char *msg);
void print_tokens(const char *input);

#endif
