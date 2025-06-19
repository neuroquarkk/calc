#include "../include/parser.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Init parser with lexer
Parser_t *parser_init(Lexer_t *lexer) {
    Parser_t *parser = malloc(sizeof(Parser_t));
    if (!parser) {
        fprintf(stderr, "Error: Memory allocation failed for parser\n");
        return NULL;
    }

    parser->lexer = lexer;
    parser->curr_token = lexer_next_token(lexer); // Load the first token

    return parser;
}

// Clean up parser memory
void parser_free(Parser_t *parser) {
    if (parser) {
        if (parser->curr_token) {
            token_free(parser->curr_token);
        }
        free(parser);
    }
}

// Recursively free AST nodes and all its children
void ast_free(ASTNode_t *node) {
    if (!node)
        return;

    switch (node->type) {
    case AST_BINARY_OP:
        ast_free(node->data.binary_op.left);
        ast_free(node->data.binary_op.right);
        break;
    case AST_UNARY_OP:
        ast_free(node->data.unary_op.operand);
        break;
    case AST_NUMBER:
        break;
    }

    free(node);
}

// Move to next token if current one matches expected type
static void eat(Parser_t *parser, TokenType expected) {
    if (parser->curr_token->type == expected) {
        token_free(parser->curr_token);
        parser->curr_token = lexer_next_token(parser->lexer);
    } else {
        fprintf(stderr, "Paser Error: Expected token type %s, get %s\n",
                token_type_to_string(expected),
                token_type_to_string(parser->curr_token->type));
        exit(1);
    }
}

// Create a number node from a give value
static ASTNode_t *create_number_node(double value) {
    ASTNode_t *node = malloc(sizeof(ASTNode_t));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for AST node\n");
        return NULL;
    }

    node->type = AST_NUMBER;
    node->data.number = value;

    return node;
}

// Create a binary operator node (+, -, *, /, ^)
static ASTNode_t *create_binary_node(TokenType op, ASTNode_t *left, ASTNode_t *right) {
    ASTNode_t *node = malloc(sizeof(ASTNode_t));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for AST node\n");
        return NULL;
    }

    node->type = AST_BINARY_OP;
    node->data.binary_op.op = op;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;

    return node;
}

// Create a unary operator node (+, -)
static ASTNode_t *create_unary_node(TokenType op, ASTNode_t *operand) {
    ASTNode_t *node = malloc(sizeof(ASTNode_t));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for AST node\n");
        return NULL;
    }

    node->type = AST_UNARY_OP;
    node->data.unary_op.op = op;
    node->data.unary_op.operand = operand;

    return node;
}

// Parse primary elements (numbers and parans)
ASTNode_t *parse_primary(Parser_t *parser) {
    Token_t *token = parser->curr_token;

    if (token->type == TOKEN_NUMBER) {
        double value = atof(token->value);
        eat(parser, TOKEN_NUMBER);
        return create_number_node(value);
    }

    if (token->type == TOKEN_LPAREN) {
        eat(parser, TOKEN_LPAREN);
        ASTNode_t *node = parse_expression(parser);
        eat(parser, TOKEN_RPAREN);
        return node;
    }

    parser_error(parser, "Expected number or '('");
    return NULL;
}

// Parse exponentiation, right associative power
ASTNode_t *parser_power(Parser_t *parser) {
    ASTNode_t *left = parse_primary(parser);

    if (parser->curr_token->type == TOKEN_POWER) {
        TokenType op = parser->curr_token->type;
        eat(parser, TOKEN_POWER);
        ASTNode_t *right = parser_power(parser); // right associative
        return create_binary_node(op, left, right);
    }

    return left;
}

// Parse unary operations (+, -)
ASTNode_t *parser_factor(Parser_t *parser) {
    Token_t *token = parser->curr_token;

    if (token->type == TOKEN_MINUS) {
        eat(parser, TOKEN_MINUS);
        ASTNode_t *operand = parser_factor(parser);
        return create_unary_node(TOKEN_MINUS, operand);
    }

    if (token->type == TOKEN_PLUS) {
        eat(parser, TOKEN_PLUS);
        ASTNode_t *operand = parser_factor(parser);
        return create_unary_node(TOKEN_PLUS, operand);
    }

    return parser_power(parser);
}

// Parse multiplication and division, left associative
ASTNode_t *parse_term(Parser_t *parser) {
    ASTNode_t *left = parser_factor(parser);

    while (parser->curr_token->type == TOKEN_MULTIPLY ||
           parser->curr_token->type == TOKEN_DIVIDE) {
        TokenType op = parser->curr_token->type;
        eat(parser, op);
        ASTNode_t *right = parser_factor(parser);
        left = create_binary_node(op, left, right);
    }

    return left;
}

// Parse addition and subtraction, left associative
ASTNode_t *parse_expression(Parser_t *parser) {
    ASTNode_t *left = parse_term(parser);

    while (parser->curr_token->type == TOKEN_PLUS ||
           parser->curr_token->type == TOKEN_MINUS) {
        TokenType op = parser->curr_token->type;
        eat(parser, op);
        ASTNode_t *right = parse_term(parser);
        left = create_binary_node(op, left, right);
    }

    return left;
}

// Main parsing function, constructs full AST from input
ASTNode_t *parser_parse(Parser_t *parser) {
    ASTNode_t *ast = parse_expression(parser);

    if (parser->curr_token->type != TOKEN_EOF) {
        parser_error(parser, "Unexpected token after expression");
        ast_free(ast);
        return NULL;
    }

    return ast;
}

// Recursively evaluates the AST
double ast_eval(ASTNode_t *node) {
    if (!node) {
        fprintf(stderr, "Error: NULL AST node\n");
        return 0.0;
    }

    switch (node->type) {
    case AST_NUMBER:
        return node->data.number;

    case AST_BINARY_OP: {
        double left_val = ast_eval(node->data.binary_op.left);
        double right_val = ast_eval(node->data.binary_op.right);

        switch (node->data.binary_op.op) {
        case TOKEN_PLUS:
            return left_val + right_val;
        case TOKEN_MINUS:
            return left_val - right_val;
        case TOKEN_MULTIPLY:
            return left_val * right_val;
        case TOKEN_DIVIDE:
            if (right_val == 0.0) {
                fprintf(stderr, "Error: Division by zero\n");
                return 0;
            }
            return left_val / right_val;
        case TOKEN_POWER:
            return pow(left_val, right_val);
        default:
            fprintf(stderr, "Error: Unknown binary operator\n");
            return 0.0;
        }
    }

    case AST_UNARY_OP: {
        double operand_val = ast_eval(node->data.unary_op.operand);

        switch (node->data.unary_op.op) {
        case TOKEN_MINUS:
            return -operand_val;
        case TOKEN_PLUS:
            return operand_val;
        default:
            fprintf(stderr, "Error: Unknown unary operator\n");
            return 0.0;
        }
    }

    default:
        fprintf(stderr, "Error: Unknown AST node type\n");
        return 0.0;
    }
}

// Print AST tree for debugging
void ast_print(ASTNode_t *node, int indent) {
    if (!node)
        return;

    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    switch (node->type) {
    case AST_NUMBER:
        printf("NUMBER: %0.2f\n", node->data.number);
        break;

    case AST_BINARY_OP:
        printf("BINARY_OP: %s\n", token_type_to_string(node->data.binary_op.op));
        ast_print(node->data.binary_op.left, indent + 1);
        ast_print(node->data.binary_op.right, indent + 1);
        break;

    case AST_UNARY_OP:
        printf("UNARY OP: %s\n", token_type_to_string(node->data.unary_op.op));
        ast_print(node->data.unary_op.operand, indent + 1);
        break;
    }
}

// Print a parser error message
void parser_error(Parser_t *parser, const char *msg) {
    fprintf(stderr, "Parser Error: %s... Current token: %s", msg,
            token_type_to_string(parser->curr_token->type));

    if (parser->curr_token->value) {
        fprintf(stderr, "(%s)", parser->curr_token->value);
    }

    fprintf(stderr, "\n");
}
