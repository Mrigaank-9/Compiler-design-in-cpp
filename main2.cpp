#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

enum class TokenType {
    INT, FLOAT, IDENTIFIER, PLUS, MINUS, MUL, DIV, ASSIGN, SEMICOLON,
    LPAREN, RPAREN, LBRACE, RBRACE, IF, ELSE, WHILE, PRINT, END, ERROR, GREATER_THAN
};

struct Token {
    TokenType type;
    string lexeme;
};



class Lexer {
public:
    Lexer(string input) : input(input), current_pos(0) {}

    Token getNextToken() {
        while (current_pos < input.size() && isspace(input[current_pos])) {
            advance(); // Skip whitespace
        }

        if (current_pos >= input.size()) {
            return { TokenType::END, "" };
        }

        char current_char = input[current_pos];

        if (isdigit(current_char)) {
            return getNumberToken();
        } else if (isalpha(current_char)) {
            return getIdentifierOrKeywordToken();
        } else {
            switch (current_char) {
                case '+': advance(); return { TokenType::PLUS, "+" };
                case '-': advance(); return { TokenType::MINUS, "-" };
                case '*': advance(); return { TokenType::MUL, "*" };
                case '/': advance(); return { TokenType::DIV, "/" };
                case '=': advance(); return { TokenType::ASSIGN, "=" };
                case ';': advance(); return { TokenType::SEMICOLON, ";" };
                case '(': advance(); return { TokenType::LPAREN, "(" };
                case ')': advance(); return { TokenType::RPAREN, ")" };
                case '{': advance(); return { TokenType::LBRACE, "{" };
                case '}': advance(); return { TokenType::RBRACE, "}" };
                case '>': advance(); return { TokenType::GREATER_THAN, ">" };
                default:
                    advance();
                    return { TokenType::ERROR, string(1, current_char) };
            }
        }
    }

private:
    void advance() {
        current_pos++;
    }

    Token getNumberToken() {
        string num;
        bool is_float = false;
        while (current_pos < input.size() && (isdigit(input[current_pos]) || input[current_pos] == '.')) {
            if (input[current_pos] == '.') {
                if (is_float) {
                    break; // Second dot found, stop parsing number
                }
                is_float = true;
            }
            num += input[current_pos];
            advance();
        }
        if (is_float) {
            return { TokenType::FLOAT, num };
        }
        return { TokenType::INT, num };
    }

    Token getIdentifierOrKeywordToken() {
        string identifier;
        while (current_pos < input.size() && (isalnum(input[current_pos]) || input[current_pos] == '_')) {
            identifier += input[current_pos];
            advance();
        }
        if (identifier == "if") {
            return { TokenType::IF, "if" };
        } else if (identifier == "else") {
            return { TokenType::ELSE, "else" };
        } else if (identifier == "while") {
            return { TokenType::WHILE, "while" };
        } else if (identifier == "print") {
            return { TokenType::PRINT, "print" };
        } else if (identifier == "int") {
            return { TokenType::INT, "int" };
        } else if (identifier == "float") {
            return { TokenType::FLOAT, "float" };
        }
        return { TokenType::IDENTIFIER, identifier };
    }

    string input;
    size_t current_pos;
};

class Parser {
public:
    Parser(Lexer& lexer, ostream& output) : lexer(lexer), current_token(lexer.getNextToken()), output(output) {}

    void parse() {
        while (current_token.type != TokenType::END && current_token.type != TokenType::ERROR) {
            if (current_token.type == TokenType::INT || current_token.type == TokenType::FLOAT) {
                parseVariableDeclaration();
            } else if (current_token.type == TokenType::IDENTIFIER) {
                parseAssignmentOrFunctionCall();
            } else if (current_token.type == TokenType::IF) {
                parseIfStatement();
            } else if (current_token.type == TokenType::WHILE) {
                parseWhileLoop();
            } else if (current_token.type == TokenType::PRINT) {
                parsePrintStatement();
            } else {
                cerr << "Syntax error: Unexpected token '" << current_token.lexeme << "'" << endl;
                advance();
            }
        }
    }

private:
    void parseVariableDeclaration() {
        Token type_token = current_token;
        advance();
        if (current_token.type == TokenType::IDENTIFIER) {
            Token identifier_token = current_token;
            advance();

            if (current_token.type == TokenType::ASSIGN) {
                advance();
                parseExpression();
                output << "mov " << identifier_token.lexeme << ", eax" << endl;
            } else if (current_token.type == TokenType::SEMICOLON) {
                output << "var " << identifier_token.lexeme << endl;
            }
            if (current_token.type == TokenType::SEMICOLON) {
                advance();
            }
        } else {
            cerr << "Syntax error: Expected identifier after type" << endl;
        }
    }

    void parseAssignmentOrFunctionCall() {
        Token identifier_token = current_token;
        advance(); 

        if (current_token.type == TokenType::ASSIGN) {
            advance(); 
            parseExpression();
            output << "mov " << identifier_token.lexeme << ", eax" << endl;
            if (current_token.type == TokenType::SEMICOLON) {
                advance();
            }
        } else if (current_token.type == TokenType::LPAREN) {
            advance();
            while (current_token.type != TokenType::RPAREN && current_token.type != TokenType::END) {
                advance(); 
            }
            if (current_token.type == TokenType::RPAREN) {
                advance(); 
            }
            output << "call " << identifier_token.lexeme << endl;
            if (current_token.type == TokenType::SEMICOLON) {
                advance(); 
            }
        } else {
            cerr << "Syntax error: Unexpected token after identifier '" << identifier_token.lexeme << "'" << endl;
        }
    }

    void parseIfStatement() {
        advance();
        if (current_token.type == TokenType::LPAREN) {
            advance(); 
            parseExpression();
            if (current_token.type == TokenType::RPAREN) {
                advance(); 
                output << "if eax != 0 jump label" << endl;
                parseBlock();
                if (current_token.type == TokenType::ELSE) {
                    advance(); 
                    output << "label:" << endl;
                    parseBlock();
                }
            } else {
                cerr << "Syntax error: Expected ')' after condition in if statement" << endl;
            }
        } else {
            cerr << "Syntax error: Expected '(' after 'if'" << endl;
        }
    }

    void parseWhileLoop() {
        advance();
        output << "label:" << endl;
        if (current_token.type == TokenType::LPAREN) {
            advance(); 
            parseExpression();
            if (current_token.type == TokenType::RPAREN) {
                advance(); 
                output << "if eax == 0 jump end_label" << endl;
                parseBlock();
                output << "jump label" << endl;
                output << "end_label:" << endl;
            } else {
                cerr << "Syntax error: Expected ')' after condition in while statement" << endl;
            }
        } else {
            cerr << "Syntax error: Expected '(' after 'while'" << endl;
        }
    }

    void parsePrintStatement() {
        advance(); 
        if (current_token.type != TokenType::LPAREN) {
            cerr << "Syntax error: Expected '(' after 'print'" << endl;
            return;
        }
        advance(); 
        parseExpression();
        output << "print eax" << endl;
        if (current_token.type != TokenType::RPAREN) {
            cerr << "Syntax error: Expected ')' after print statement" << endl;
        } else {
            advance(); 
        }
        if (current_token.type == TokenType::SEMICOLON) {
            advance(); 
        }
    }

    void parseBlock() {
        if (current_token.type == TokenType::LBRACE) {
            advance(); 
            while (current_token.type != TokenType::RBRACE && current_token.type != TokenType::END) {
                if (current_token.type == TokenType::ERROR) {
                    cerr << "Syntax error: Unexpected token '" << current_token.lexeme << "'" << endl;
                    advance();
                } else {
                    parseStatement();
                }
            }
            if (current_token.type == TokenType::RBRACE) {
                advance(); 
            } else {
                cerr << "Syntax error: Expected '}' at end of block" << endl;
            }
        } else {
            parseStatement();
        }
    }

    void parseStatement() {
        if (current_token.type == TokenType::END) return;
        else if (current_token.type == TokenType::ERROR) {
            cerr << "Syntax error: Unexpected token '" << current_token.lexeme << "'" << endl;
            advance();
        } else if (current_token.type == TokenType::INT || current_token.type == TokenType::FLOAT) {
            parseVariableDeclaration();
        } else if (current_token.type == TokenType::IDENTIFIER) {
            parseAssignmentOrFunctionCall();
        } else if (current_token.type == TokenType::IF) {
            parseIfStatement();
        } else if (current_token.type == TokenType::WHILE) {
            parseWhileLoop();
        } else if (current_token.type == TokenType::PRINT) {
            parsePrintStatement();
        } else {
            cerr << "Syntax error: Unexpected token '" << current_token.lexeme << "'" << endl;
            advance();
        }
    }

    void parseExpression() {
        if (current_token.type == TokenType::IDENTIFIER || current_token.type == TokenType::INT || current_token.type == TokenType::FLOAT) {
            output << "load " << current_token.lexeme << " into eax" << endl;
            advance();
        }
        if (current_token.type == TokenType::GREATER_THAN) {
            Token op = current_token;
            advance();
            output << "compare eax with " << current_token.lexeme << endl;
            advance();
        }
        else if(current_token.type == TokenType::MINUS){
            Token op = current_token;
            advance();
            output << "substract eax with " << current_token.lexeme << endl;
            advance();
        }
        else if(current_token.type == TokenType::PLUS){
            Token op = current_token;
            advance();
            output << "Add eax with " << current_token.lexeme << endl;
            advance();
        }
        // else if(current_token.type == TokenType ::  )
    }

    void advance() {
        current_token = lexer.getNextToken();
    }

    Lexer& lexer;
    Token current_token;
    ostream& output;
};

int main() {
    string input_code =
        ""
        "   int x = 10;"
        "   int y = 20;"
        "   if (x > y) {"
        "       print(x);"
        "   } else {"
        "       print(y);"
        "   }"
        "   while (x > 0) {"
        "       x = x - 1;"
        "   } " 
        "";

    ofstream outputFile("output.asm");

    Lexer lexer(input_code);
    Parser parser(lexer, outputFile);

    parser.parse();

    outputFile.close();

    cout << "Execution completed  " << endl; 

    return 0;
}
