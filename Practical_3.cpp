#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <unordered_set>
#include <unordered_map>
using namespace std;

// Defining C keywords
unordered_set<string> keywords = {"int", "char", "return", "if", "else", "while", "for", "break", "continue"};

// Defining operators and punctuation
unordered_set<char> operators = {'+', '-', '*', '/', '=', '<', '>', '!'};
unordered_set<char> punctuation = {';', ',', '{', '}', '(', ')'};

// Symbol table
unordered_map<string, int> symbolTable;

// Function to check if a string is a keyword or not
bool isKeyword(const string &word) {
    return keywords.find(word) != keywords.end();
}

// Function to check if a token is a valid identifier or not
bool isValidIdentifier(const string &token) {
    if (isdigit(token[0])) return false;
    for (char c : token) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

// Function to process tokens
void processToken(const string &token, int lineNum, vector<string> &tokens, vector<string> &errors) {
    if (token.empty()) return;

    if (isKeyword(token)) {
        tokens.push_back("Keyword: " + token);
    } else if (isdigit(token[0])) {
        // Check if token is a valid number
        bool valid = true;
        for (char c : token) {
            if (!isdigit(c) && !isalpha(c)) {
                valid = false;
                break;
            }
        }
        if (valid)
            tokens.push_back("Constant: " + token);
        else
            errors.push_back("Line " + to_string(lineNum) + ": " + token + " invalid lexeme");
    } else if (isValidIdentifier(token)) {
        // Identifier processing
        tokens.push_back("Identifier: " + token);
        symbolTable[token]++;
    } else {
        errors.push_back("Line " + to_string(lineNum) + ": " + token + " invalid identifier");
    }
}

// Lexical analyzer function
void lexicalAnalyzer(const string &filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open file." << endl;
        return;
    }

    vector<string> tokens;
    vector<string> errors;
    string token = "";
    char ch;
    int lineNum = 1;
    bool inComment = false;

    while (file.get(ch)) {
        // Handling new lines
        if (ch == '\n') {
            lineNum++;
            continue;
        }

        // Handling comments
        if (inComment) {
            if (ch == '*' && file.peek() == '/') {
                file.get(ch);
                inComment = false;
            }
            continue;
        }
        if (ch == '/' && file.peek() == '*') {
            file.get(ch);
            inComment = true;
            continue;
        }
        if (ch == '/' && file.peek() == '/') {
            while (file.get(ch) && ch != '\n');
            lineNum++;
            continue;
        }

        // Handling tokens
        if (isalnum(ch) || ch == '_') {
            token += ch;
        } else {
            processToken(token, lineNum, tokens, errors);
            token = "";

            if (operators.count(ch)) {
                tokens.push_back("Operator: " + string(1, ch));
            } else if (punctuation.count(ch)) {
                tokens.push_back("Punctuation: " + string(1, ch));
            } else if (!isspace(ch)) {
                errors.push_back("Line " + to_string(lineNum) + ": " + string(1, ch) + " invalid character");
            }
        }
    }
    processToken(token, lineNum, tokens, errors);

    file.close();

    // Printing tokens
    cout << "TOKENS\n";
    for (const string &t : tokens) {
        cout << t << endl;
    }

    // Printing errors
    if (!errors.empty()) {
        cout << "\nLEXICAL ERRORS\n";
        for (const string &e : errors) {
            cout << e << endl;
        }
    }

    // Printing symbol table
    cout << "\nSYMBOL TABLE ENTRIES\n";
    int index = 1;
    for (const auto &entry : symbolTable) {
        cout << index++ << ") " << entry.first << endl;
    }
}

int main() {
    string filename = "test.c"; // give your C file here with extension(.c)
    lexicalAnalyzer(filename);
    return 0;
}