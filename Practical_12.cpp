#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <iomanip>
using namespace std;

// Token types.
enum class TokenType { Number, Identifier, Operator, LParen, RParen, End };

struct Token {
    TokenType type;
    string text;
};

class Lexer {
public:
    Lexer(const string &input) : input(input), pos(0) {}
    
    Token getNextToken() {
        while (pos < input.size() && isspace(input[pos]))
            pos++;
        if (pos >= input.size())
            return {TokenType::End, ""};
        
        char cur = input[pos];
        if (isdigit(cur) || cur == '.') { // number (integer or decimal)
            int start = pos;
            bool dotFound = false;
            while (pos < input.size() && (isdigit(input[pos]) || input[pos]=='.')) {
                if (input[pos]=='.') {
                    if (dotFound)
                        break;
                    dotFound = true;
                }
                pos++;
            }
            return {TokenType::Number, input.substr(start, pos - start)};
        }
        if (isalpha(cur)) { // variable identifier
            int start = pos;
            while (pos < input.size() && isalnum(input[pos]))
                pos++;
            return {TokenType::Identifier, input.substr(start, pos - start)};
        }
        // Operators or parentheses:
        pos++;
        if (cur == '+' || cur == '-' || cur == '*' || cur == '/' || cur == '^')
            return {TokenType::Operator, string(1,cur)};
        if (cur == '(')
            return {TokenType::LParen, "("};
        if (cur == ')')
            return {TokenType::RParen, ")"};
        throw runtime_error("Unknown character: " + string(1, cur));
    }
    
private:
    string input;
    size_t pos;
};

// Forward declaration for AST
class Expr;
using ExprPtr = shared_ptr<Expr>;

enum class NodeType { Constant, Variable, Binary };

// Base class for AST nodes.
class Expr {
public:
    virtual ~Expr() = default;
    virtual bool isConstant() const = 0;
    virtual double getConstantValue() const { throw runtime_error("Not a constant"); }
    virtual string toString() const = 0;
    // Method to get precedence for proper parenthesization
    virtual int getPrecedence() const { return 0; }
};

class ConstantExpr : public Expr {
public:
    ConstantExpr(double value) : value(value) {}
    bool isConstant() const override { return true; }
    double getConstantValue() const override { return value; }
    string toString() const override {
        // Remove trailing zeros if integer.
        ostringstream oss;
        if(floor(value) == value)
            oss << static_cast<int>(value);
        else
            oss << fixed << setprecision(6) << value;
        return oss.str();
    }
    int getPrecedence() const override { return 100; } // Highest precedence
private:
    double value;
};

class VariableExpr : public Expr {
public:
    VariableExpr(const string &name) : name(name) {}
    bool isConstant() const override { return false; }
    string toString() const override { return name; }
    int getPrecedence() const override { return 100; } // Highest precedence
private:
    string name;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(const string &op, ExprPtr left, ExprPtr right)
        : op(op), left(left), right(right) {}
    bool isConstant() const override {
        return left->isConstant() && right->isConstant();
    }
    double getConstantValue() const override {
        double l = left->getConstantValue();
        double r = right->getConstantValue();
        if (op == "+") return l + r;
        else if (op == "-") return l - r;
        else if (op == "*") return l * r;
        else if (op == "/") {
            if (r == 0) throw runtime_error("Division by zero");
            return l / r;
        }
        else if (op == "^") return pow(l, r);
        throw runtime_error("Unknown operator in constant folding: " + op);
    }
    int getPrecedence() const override {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        if (op == "^") return 3;
        return 0;
    }
    
    string toString() const override {
        // More intelligent parenthesization based on operator precedence
        bool needLeftParen = left->getPrecedence() < getPrecedence();
        bool needRightParen = right->getPrecedence() < getPrecedence() || 
                             (right->getPrecedence() == getPrecedence() && op == "-");
                             
        string leftStr = needLeftParen ? "(" + left->toString() + ")" : left->toString();
        string rightStr = needRightParen ? "(" + right->toString() + ")" : right->toString();
        
        return leftStr + " " + op + " " + rightStr;
    }
    
    string op;
    ExprPtr left, right;
};

// Recursive descent parser.
class Parser {
public:
    Parser(const string &input) : lexer(input) {
        curToken = lexer.getNextToken();
    }
    
    ExprPtr parseExpression() {
        return parseAddSub();
    }
    
private:
    Lexer lexer;
    Token curToken;
    
    void eat(TokenType type) {
        if (curToken.type == type)
            curToken = lexer.getNextToken();
        else
            throw runtime_error("Unexpected token: " + curToken.text);
    }
    
    // Operator precedence:
    // Level 1: +, -
    // Level 2: *, /
    // Level 3: ^ (right associative)
    // Level 4: unary + and -
    ExprPtr parseAddSub() {
        ExprPtr node = parseMulDiv();
        while (curToken.type == TokenType::Operator && (curToken.text=="+" || curToken.text=="-")) {
            string op = curToken.text;
            eat(TokenType::Operator);
            ExprPtr right = parseMulDiv();
            node = make_shared<BinaryExpr>(op, node, right);
        }
        return node;
    }
    
    ExprPtr parseMulDiv() {
        ExprPtr node = parseExponent();
        while (curToken.type == TokenType::Operator && (curToken.text=="*" || curToken.text=="/")) {
            string op = curToken.text;
            eat(TokenType::Operator);
            ExprPtr right = parseExponent();
            node = make_shared<BinaryExpr>(op, node, right);
        }
        return node;
    }
    
    // Exponentiation is right-associative.
    ExprPtr parseExponent() {
        ExprPtr node = parsePrimary();
        if (curToken.type == TokenType::Operator && curToken.text=="^") {
            string op = curToken.text;
            eat(TokenType::Operator);
            ExprPtr right = parseExponent();
            node = make_shared<BinaryExpr>(op, node, right);
        }
        return node;
    }
    
    ExprPtr parsePrimary() {
        if (curToken.type == TokenType::Number) {
            double value = stod(curToken.text);
            eat(TokenType::Number);
            return make_shared<ConstantExpr>(value);
        }
        if (curToken.type == TokenType::Identifier) {
            string name = curToken.text;
            eat(TokenType::Identifier);
            return make_shared<VariableExpr>(name);
        }
        if (curToken.type == TokenType::LParen) {
            eat(TokenType::LParen);
            ExprPtr node = parseExpression();
            eat(TokenType::RParen);
            return node;
        }
        throw runtime_error("Unexpected token in primary: " + curToken.text);
    }
};

// Advanced constant folding that combines constants across operations
ExprPtr constantFold(ExprPtr root) {
    if (!root) return nullptr;

    if (auto bin = dynamic_pointer_cast<BinaryExpr>(root)) {
        // First fold children
        bin->left = constantFold(bin->left);
        bin->right = constantFold(bin->right);

        cout << "Checking: " << bin->toString() << "\n";

        // If both children are constants, fold it
        if (bin->left->isConstant() && bin->right->isConstant()) {
            double value = bin->getConstantValue();
            cout << "Folding: " << bin->toString() << " -> " << value << endl;
            return make_shared<ConstantExpr>(value);
        }
        
        // Advanced optimization for expressions with a mix of variables and constants
        // Handle cases like "x + 5 + 3" -> "x + 8"
        auto tryOptimizeBinaryOp = [](BinaryExpr* expr) -> ExprPtr {
            // For addition: (x + const1) + const2 -> x + (const1 + const2)
            if (expr->op == "+") {
                auto leftBin = dynamic_pointer_cast<BinaryExpr>(expr->left);
                if (leftBin && leftBin->op == "+" && !leftBin->left->isConstant() && leftBin->right->isConstant() && expr->right->isConstant()) {
                    double combined = leftBin->right->getConstantValue() + expr->right->getConstantValue();
                    cout << "Combining constants: " << leftBin->right->toString() << " + " << expr->right->toString() << " -> " << combined << endl;
                    return make_shared<BinaryExpr>("+", leftBin->left, make_shared<ConstantExpr>(combined));
                }
                
                // For cases like const + (x + const)
                if (expr->left->isConstant() && !expr->right->isConstant()) {
                    auto rightBin = dynamic_pointer_cast<BinaryExpr>(expr->right);
                    if (rightBin && rightBin->op == "+" && !rightBin->left->isConstant() && rightBin->right->isConstant()) {
                        double combined = expr->left->getConstantValue() + rightBin->right->getConstantValue();
                        cout << "Combining constants: " << expr->left->toString() << " + " << rightBin->right->toString() << " -> " << combined << endl;
                        return make_shared<BinaryExpr>("+", rightBin->left, make_shared<ConstantExpr>(combined));
                    }
                }
            }
            // For subtraction: (x + const1) - const2 -> x + (const1 - const2)
            else if (expr->op == "-") {
                auto leftBin = dynamic_pointer_cast<BinaryExpr>(expr->left);
                if (leftBin && leftBin->op == "+" && !leftBin->left->isConstant() && leftBin->right->isConstant() && expr->right->isConstant()) {
                    double combined = leftBin->right->getConstantValue() - expr->right->getConstantValue();
                    cout << "Combining constants: " << leftBin->right->toString() << " - " << expr->right->toString() << " -> " << combined << endl;
                    return make_shared<BinaryExpr>("+", leftBin->left, make_shared<ConstantExpr>(combined));
                }
                
                // For cases like const - (x - const)
                if (expr->left->isConstant() && !expr->right->isConstant()) {
                    auto rightBin = dynamic_pointer_cast<BinaryExpr>(expr->right);
                    if (rightBin && rightBin->op == "-" && !rightBin->left->isConstant() && rightBin->right->isConstant()) {
                        double combined = expr->left->getConstantValue() + rightBin->right->getConstantValue();
                        cout << "Combining constants: " << expr->left->toString() << " + " << rightBin->right->toString() << " -> " << combined << endl;
                        return make_shared<BinaryExpr>("-", make_shared<ConstantExpr>(combined), rightBin->left);
                    }
                }
            }
            
            return nullptr;
        };
        
        // Try to apply the advanced optimization
        ExprPtr optimized = tryOptimizeBinaryOp(bin.get());
        if (optimized) {
            // Recursively optimize the result again
            return constantFold(optimized);
        }
        
        return bin;
    }

    return root;
}

int main() {
    ios::sync_with_stdio(false);
    
    cout << "Enter arithmetic expression:" << "\n";
    string input;
    getline(cin, input);
    
    try {
        Parser parser(input);
        ExprPtr ast = parser.parseExpression();
        // Apply constant folding.
        ExprPtr optimized = constantFold(ast);
        // Get the optimized expression as a string
        string optStr = optimized->toString();
        
        cout << "Optimized expression: " << optStr << "\n";
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << "\n";
    }
    
    return 0;
}