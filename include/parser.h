#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// AST Node types for different kinds of expression
typedef enum {
    AST_NUMBER,    // Node containing a number
    AST_BINARY_OP, // Binary operations (+, -, *, /, ^)
    AST_UNARY_OP,  // Unary operations (-, +)
} ASTNodeType;

// Forward declaration of the AST node structure
typedef struct ASTNode ASTNode_t;

// Structure representing a node in the AST
struct ASTNode {
    ASTNodeType type; // Kind of node
    union {
        double number; // Used if node is a number
        struct {
            TokenType op;     // Binary operator (+, -, *, /, ^)
            ASTNode_t *left;  // Left operand
            ASTNode_t *right; // Right operand
        } binary_op;
        struct {
            TokenType op;       // Unary operator (+, -)
            ASTNode_t *operand; // Operand
        } unary_op;
    } data;
};

// Parser state structure including current token
typedef struct {
    Lexer_t *lexer;
    Token_t *curr_token;
} Parser_t;

// Parser function declarations
Parser_t *parser_init(Lexer_t *lexer);
void parser_free(Parser_t *parser);
void ast_free(ASTNode_t *node);
ASTNode_t *parser_parse(Parser_t *parser);
double ast_eval(ASTNode_t *node);
void parser_error(Parser_t *parser, const char *msg);
void ast_print(ASTNode_t *node, int indent);

ASTNode_t *parse_expression(Parser_t *parser);
ASTNode_t *parser_term(Parser_t *parser);
ASTNode_t *parse_factor(Parser_t *parser);
ASTNode_t *parser_power(Parser_t *parser);
ASTNode_t *parse_primary(Parser_t *parser);

#endif
