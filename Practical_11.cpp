#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

struct Quadruple {
    string op;
    string arg1;
    string arg2;
    string result;
};

int tempCount = 1;
vector<Quadruple> quadruples;

string getTempVar() {
    return "t" + to_string(tempCount++);
}

// Function to remove spaces
string removeSpaces(const string &str) {
    string result = "";
    for (char ch : str) {
        if (!isspace(ch))
            result += ch;
    }
    return result;
}

// Function to extract a number from the expression
string extractNumber(const string &expr, int &i) {
    string num = "";
    while (i < expr.length() && isdigit(expr[i])) {
        num += expr[i];
        i++;
    }
    i--; // Adjust index
    return num;
}

// Function to handle parenthesis recursively
string processExpression(string expr, int &i);

// Function to handle multiplication and division
string processTerm(string expr, int &i) {
    string left;
    if (expr[i] == '(') {
        i++; // Skip '('
        left = processExpression(expr, i);
        i++; // Skip ')'
    } else {
        left = extractNumber(expr, i);
    }

    while (i + 1 < expr.length() && (expr[i + 1] == '*' || expr[i + 1] == '/')) {
        char op = expr[++i];
        i++;
        string right;

        if (expr[i] == '(') {
            i++;
            right = processExpression(expr, i);
            i++;
        } else {
            right = extractNumber(expr, i);
        }

        string temp = getTempVar();
        quadruples.push_back({string(1, op), left, right, temp});
        left = temp;
    }
    return left;
}

// Function to handle addition and subtraction
string processExpression(string expr, int &i) {
    string left = processTerm(expr, i);

    while (i + 1 < expr.length() && (expr[i + 1] == '+' || expr[i + 1] == '-')) {
        char op = expr[++i];
        i++;
        string right = processTerm(expr, i);

        string temp = getTempVar();
        quadruples.push_back({string(1, op), left, right, temp});
        left = temp;
    }
    return left;
}

int main() {
    string expression;
    cout << "Enter an arithmetic expression (integers only): ";
    getline(cin, expression);

    expression = removeSpaces(expression);
    int i = 0;
    processExpression(expression, i);

    // Output Quadruple Table
    cout << "\nQuadruple Table:\n";
    cout << "Op\tArg1\tArg2\tResult\n";
    for (auto q : quadruples) {
        cout << q.op << "\t" << q.arg1 << "\t" << q.arg2 << "\t" << q.result << endl;
    }
    return 0;
}