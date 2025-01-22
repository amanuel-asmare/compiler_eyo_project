#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define token types
typedef enum {
    KEYWORD, OPERATOR, DELIMITER, IDENTIFIER, LITERAL, COMMENT, ERROR
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char value[50];
    int line;
} Token;

// Keywords
const char* keywords[] = {"if", "else", "while", "for", "return", "int", "float", "char", "void"};

// Operators
const char* operators[] = {"+", "-", "*", "/", "=", "==", "<", ">", "<=", ">=", "&&", "||", "!"};

// Delimiters
const char delimiters[] = "{}()[].,;";

// Function to check if a string is a keyword
int isKeyword(char* str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a character is a delimiter
int isDelimiter(char ch) {
    for (int i = 0; i < sizeof(delimiters) / sizeof(char); i++) {
        if (ch == delimiters[i]) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a string is an operator
int isOperator(char* str) {
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
        if (strcmp(str, operators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to display tokens
void displayTokens(Token* tokens, int tokenCount) {
    printf("%-15s %-15s %s\n", "Token Type", "Token Value", "Line Number");
    printf("=============================================\n");
    for (int i = 0; i < tokenCount; i++) {
        const char* type_str;
        switch (tokens[i].type) {
            case KEYWORD: type_str = "Keyword"; break;
            case OPERATOR: type_str = "Operator"; break;
            case DELIMITER: type_str = "Delimiter"; break;
            case IDENTIFIER: type_str = "Identifier"; break;
            case LITERAL: type_str = "Literal"; break;
            case COMMENT: type_str = "Comment"; break;
            default: type_str = "Error"; break;
        }
        printf("%-15s %-15s %d\n", type_str, tokens[i].value, tokens[i].line);
    }
}

// Function to identify tokens in the source code
void identifyTokens(FILE* file, Token* tokens, int* tokenCount, int* lineNumber) {
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            (*lineNumber)++;
            continue;
        }

        // Ignore whitespace
        if (isspace(ch)) continue;

        // Handle comments
        if (ch == '/') {
            char next = fgetc(file);
            if (next == '/') {
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                (*lineNumber)++;
                continue;
            } else if (next == '*') {
                while ((ch = fgetc(file)) != EOF) {
                    if (ch == '*' && fgetc(file) == '/') break;
                    if (ch == '\n') (*lineNumber)++;
                }
                continue;
            } else {
                ungetc(next, file);
            }
        }

        // Handle delimiters
        if (isDelimiter(ch)) {
            tokens[*tokenCount].type = DELIMITER;
            tokens[*tokenCount].value[0] = ch;
            tokens[*tokenCount].value[1] = '\0';
            tokens[*tokenCount].line = *lineNumber;
            (*tokenCount)++;
            continue;
        }

        // Handle operators
        char buffer[3] = {ch, fgetc(file), '\0'};
        if (isOperator(buffer)) {
            tokens[*tokenCount].type = OPERATOR;
            strcpy(tokens[*tokenCount].value, buffer);
            tokens[*tokenCount].line = *lineNumber;
            (*tokenCount)++;
            continue;
        } else {
            ungetc(buffer[1], file);
            buffer[1] = '\0';
            if (isOperator(buffer)) {
                tokens[*tokenCount].type = OPERATOR;
                strcpy(tokens[*tokenCount].value, buffer);
                tokens[*tokenCount].line = *lineNumber;
                (*tokenCount)++;
                continue;
            }
        }

        // Handle literals and identifiers
        if (isdigit(ch) || isalpha(ch) || ch == '_') {
            int i = 0;
            char buffer[50] = {0};
            buffer[i++] = ch;
            while ((ch = fgetc(file)) != EOF && (isalnum(ch) || ch == '.')) {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';
            ungetc(ch, file);

            tokens[*tokenCount].type = (isdigit(buffer[0]) || buffer[0] == '\'') ? LITERAL : (isKeyword(buffer) ? KEYWORD : IDENTIFIER);
            strcpy(tokens[*tokenCount].value, buffer);
            tokens[*tokenCount].line = *lineNumber;
            (*tokenCount)++;
            continue;
        }

        // Handle invalid tokens
        tokens[*tokenCount].type = ERROR;
        tokens[*tokenCount].value[0] = ch;
        tokens[*tokenCount].value[1] = '\0';
        tokens[*tokenCount].line = *lineNumber;
        (*tokenCount)++;
    }
}

int main() {
    FILE *file = fopen("source_code.txt", "r");
    if (!file) {
        printf("Error opening file\n");
        return 1;
    }

    Token tokens[1000];
    int tokenCount = 0;
    int lineNumber = 1;

    identifyTokens(file, tokens, &tokenCount, &lineNumber);

    fclose(file);
    displayTokens(tokens, tokenCount);

    return 0;
}
