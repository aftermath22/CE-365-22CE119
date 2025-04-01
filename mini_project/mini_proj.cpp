#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <array>
#include <memory>
#include <unordered_set>
#include <cstdio>
#include <cstdlib>

// Execute an external command and capture its output.
std::string execCommand(const char* cmd) {
    #ifdef _WIN32
        FILE* pipe = _popen(cmd, "r");
    #else
        FILE* pipe = popen(cmd, "r");
    #endif
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    
    std::array<char, 128> buffer;
    std::string result;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    
    #ifdef _WIN32
        _pclose(pipe);
    #else
        pclose(pipe);
    #endif

    return result;
}

//---------------------------------------------------
// Token definitions
enum class TokenType {
    Keyword,
    Identifier,
    Punctuation,
    Operator,
    Constant,
    String,
    Other
};

struct Token {
    std::string value;
    TokenType type;
    int line;
};

//---------------------------------------------------
// Syntax tree node definitions
struct SyntaxTreeNode {
    virtual ~SyntaxTreeNode() = default;
    virtual void print(int indent = 0) const = 0;
};

struct LabelDeclNode : public SyntaxTreeNode {
    std::string name;
    int line;
    LabelDeclNode(const std::string& name, int line) : name(name), line(line) {}
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ')
                  << "[Label Declaration] Name: '" << name 
                  << "', Declared at line: " << line << "\n";
    }
};

struct GotoStmtNode : public SyntaxTreeNode {
    std::string label;
    int line;
    GotoStmtNode(const std::string& label, int line) : label(label), line(line) {}
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ')
                  << "[Goto Statement] Target Label: '" << label 
                  << "', at line: " << line << "\n";
    }
};

//---------------------------------------------------
// Structure to hold the parse result (syntax tree + errors)
struct ParseResult {
    std::vector<std::unique_ptr<SyntaxTreeNode>> syntax_tree;
    std::vector<std::string> errors;
};

//---------------------------------------------------
// Parser (syntax analyzer)
ParseResult parse(const std::vector<Token>& tokens) {
    ParseResult result;
    std::unordered_set<std::string> labels;

    // Parse tokens into syntax tree nodes.
    for (size_t i = 0; i < tokens.size();) {
        if (tokens[i].type == TokenType::Keyword && tokens[i].value == "goto") {
            if (i + 1 >= tokens.size() || tokens[i+1].type != TokenType::Identifier) {
                result.errors.push_back("Syntax error: invalid goto at line " + std::to_string(tokens[i].line));
                i++;
                continue;
            }
            std::string label = tokens[i+1].value;
            result.syntax_tree.push_back(std::make_unique<GotoStmtNode>(label, tokens[i].line));
            
            // Check for semicolon punctuation.
            if (i + 2 < tokens.size() && 
                tokens[i+2].type == TokenType::Punctuation &&
                tokens[i+2].value == ";") {
                i += 3;
            } else {
                result.errors.push_back("Warning: missing semicolon after goto at line " + std::to_string(tokens[i].line));
                i += 2;
            }
        }
        // Accept colon token as label indicator even if its type is Operator.
        else if (tokens[i].type == TokenType::Identifier && 
                 i + 1 < tokens.size() && 
                 ((tokens[i+1].type == TokenType::Punctuation || tokens[i+1].type == TokenType::Operator) &&
                  tokens[i+1].value == ":")) {
            
            std::string label = tokens[i].value;
            result.syntax_tree.push_back(std::make_unique<LabelDeclNode>(label, tokens[i].line));
            labels.insert(label);
            i += 2;
        }
        else {
            i++;
        }
    }

    // Validate goto targets.
    for (const auto& node : result.syntax_tree) {
        if (auto goto_node = dynamic_cast<GotoStmtNode*>(node.get())) {
            if (!labels.count(goto_node->label)) {
                result.errors.push_back("Error: undefined label '" + goto_node->label + "' at line " + std::to_string(goto_node->line));
            }
        }
    }

    return result;
}

//---------------------------------------------------
// Helper function to convert the tokenization output in the stringstream
std::vector<Token> convertTokenizerOutput(std::istream &is) {
    std::vector<Token> tokens;
    std::string line;

    auto trim = [](std::string s) -> std::string {
        const char* whitespace = " \t\n\r";
        size_t start = s.find_first_not_of(whitespace);
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(whitespace);
        return s.substr(start, end - start + 1);
    };

    int currentLineNumber = 1;
    while (std::getline(is, line)) {
        if (line.empty())
            continue;
        if (line.find("INVALID LEXEMES") != std::string::npos ||
            line.find("Symbol Table Entries") != std::string::npos)
        {
            break;
        }
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;
        std::string typeStr = trim(line.substr(0, colonPos));
        std::string tokenValue = trim(line.substr(colonPos + 1));
        
        TokenType type;
        if (typeStr == "Keyword")       type = TokenType::Keyword;
        else if (typeStr == "Identifier") type = TokenType::Identifier;
        else if (typeStr == "Punctuation")type = TokenType::Punctuation;
        else if (typeStr == "Operator")   type = TokenType::Operator;
        else if (typeStr == "Constant")   type = TokenType::Constant;
        else if (typeStr == "String")     type = TokenType::String;
        else                            type = TokenType::Other;
        
        tokens.push_back({tokenValue, type, currentLineNumber});
        currentLineNumber++;
    }
    return tokens;
}

//---------------------------------------------------
// Function to print the entire syntax tree in a structured way.
void printSyntaxTree(const std::vector<std::unique_ptr<SyntaxTreeNode>>& syntax_tree) {
    std::cout << "\n=== Syntax Tree ===\n";
    for (const auto& node : syntax_tree) {
        node->print(2); // indent each node by 2 spaces for clarity
    }
    std::cout << "===================\n";
}

//---------------------------------------------------
// Function to print collected error messages.
void printErrors(const std::vector<std::string>& errors) {
    if (errors.empty()) {
        std::cout << "No errors found.\n";
        return;
    }
    std::cout << "\n=== Errors ===\n";
    for (const auto& error : errors) {
        std::cout << error << "\n";
    }
    std::cout << "==============\n";
}

//---------------------------------------------------
// Main integration function
int main(int argc, char* argv[]) {
    try {
        // Execute Practical_3.exe and capture its standard output.
        std::string output = execCommand("Practical_3.exe");
        
        // Store the output in a stringstream.
        std::stringstream ss(output);
        
        // Convert the stringstream output (from tokenization) into tokens.
        std::vector<Token> tokens = convertTokenizerOutput(ss);
        
        // Optional: print tokens for debugging.
        std::cout << "Tokens:" << std::endl;
        for (const auto& token : tokens) {
            std::cout << "Line " << token.line << ": " << token.value << " (";
            switch(token.type) {
                case TokenType::Keyword:       std::cout << "Keyword"; break;
                case TokenType::Identifier:    std::cout << "Identifier"; break;
                case TokenType::Punctuation:   std::cout << "Punctuation"; break;
                case TokenType::Operator:      std::cout << "Operator"; break;
                case TokenType::Constant:      std::cout << "Constant"; break;
                case TokenType::String:        std::cout << "String"; break;
                default:                       std::cout << "Other"; break;
            }
            std::cout << ")" << std::endl;
        }
        
        // Parse the tokens into a syntax tree along with error messages.
        ParseResult parseResult = parse(tokens);
        
        // Print the resulting syntax tree in a structured and more informative format.
        printSyntaxTree(parseResult.syntax_tree);
        
        // Print any errors collected during parsing.
        printErrors(parseResult.errors);
        
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
