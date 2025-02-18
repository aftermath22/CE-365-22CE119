#include <bits/stdc++.h>
#define ll long long int
#define li long int
#define pb push_back
#define all(x) x.begin(), x.end()
#define I "Identifier"
#define P "Punctuation"
#define K "Keyword"
#define C "Constant"
#define S "String"
#define O "Operator"

using namespace std;
unordered_set<string> keywords={
    "auto", "break", "case", "char", "const", "continue", "default", "do", 
    "double", "else", "enum", "extern", "float", "for", "goto", "if", 
    "inline", "int", "long", "register", "restrict", "return", "short", 
    "signed", "sizeof", "static", "struct", "switch", "typedef", "union", 
    "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", 
    "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn", 
    "_Static_assert", "_Thread_local"
};

unordered_set<string> punctuations={
    "[", "]", "(", ")", "{", "}",   // Brackets & Parentheses
    ",", ";", ":",                  // Separators
    ".", "->",                      // Structure Operators
    "#",                             // Preprocessor Directive
    "\"", "'",                       // String & Character Delimiters
    "\\",                            // Escape Character
    "?"," "                      // Rarely Used Punctuations
};

unordered_set<string> operators={
    "+", "-", "*", "/", "%",        // Arithmetic Operators
    "++", "--",                     // Increment & Decrement Operators
    "==", "!=", ">", "<", ">=", "<=", // Relational Operators
    "&&", "||", "!",                // Logical Operators
    "&", "|", "^", "~", "<<", ">>",  // Bitwise Operators
    "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", // Assignment Operators
    "?", ":",                        // Ternary Operator
    ",",                             // Comma Operator
    "sizeof",                        // sizeof Operator
    "&", "*"                         // Address-of and Pointer Dereference Operatorsz
};

bool chkident(string &s){
    std::regex identifier_regex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    return std::regex_match(s, identifier_regex);
}

bool chkconst(string &s) {
    std::regex number_regex(
        R"(^([+-]?(0[xX][0-9a-fA-F]+)"         // Hexadecimal constants (0x1A3f)
        R"(|0[0-7]+)"                          // Octal constants (0123)
        R"(|[0-9]+(ll|LL|l|L)?)"               // Decimal integers (123, 456L, 42ll)
        R"(|[0-9]*\.[0-9]+([eE][+-]?[0-9]+)?[fF]?)" // Floating-point (3.14, 2.71F)
        R"(|[0-9]+[eE][+-]?[0-9]+[fF]?))$)"    // Exponential notation (1.5e3, 4E-2f)
    );
    return std::regex_match(s, number_regex);

}


string buffer="";
bool isstr=0;
void chk(string &s,vector<pair<string,string>> &tokens,vector<string> &lexemes){
    // stringstream ss(s);
    int n=s.size();
    if(n==1){
        if(punctuations.find(s)!=punctuations.end()){
            tokens.pb({P,s});
        }
        else if(operators.find(s)!=operators.end()){
            tokens.pb({O,s});
        }
        else{
            if(chkident(s)){
                tokens.pb({I,s});
            }
            else if(chkconst(s)){
                tokens.pb({C,s});
            }
            else{
                lexemes.pb(s);
            }
        }
        return;
    }
    for(int i=0;i<n;i++){
        string curr;
        curr+=s[i];
        if(isstr){
            buffer+=curr;
            if(i-1>=0){
                if(!buffer.empty() && s[i-1]!='\\' && (s[i]==*buffer.begin() || s[i]==*buffer.begin())){
                    //string terminates
                    tokens.pb({S,buffer});
                    buffer="";
                    isstr=0;
                }
            }
        }
        else if(!isstr && (curr=="\"" || curr=="\'")){
            isstr=1;
            buffer+=curr;
            continue;
        }
        else if(!isstr){
            if(operators.find(curr)!=operators.end()){
                if(keywords.find(buffer)!=keywords.end()){
                    tokens.pb({K,buffer});
                    buffer="";
                }
                else{
                    //decide if buffer is identifier or constant or invalid
                    if(chkconst(buffer)){
                        tokens.pb({C,buffer});
                        buffer="";
                    }
                    else if(chkident(buffer)){
                        tokens.pb({I,buffer});
                        buffer="";
                    }
                    else if(!buffer.empty()){
                        lexemes.pb(buffer);
                        buffer="";
                    }
                }
                tokens.pb({O,curr});
            }
            else if(punctuations.find(curr)!=punctuations.end()){
                if(curr=="." && i-1>=0 && (isdigit(s[i-1]) || s[i-1]=='e')){
                    buffer+=curr;
                    continue;
                }
                if(keywords.find(buffer)!=keywords.end()){
                    tokens.pb({K,buffer});
                    buffer="";
                }
                else{
                    //decide if buffer is identifier or constant or invalid
                    if(chkconst(buffer)){
                        tokens.pb({C,buffer});
                        buffer="";
                    }
                    else if(chkident(buffer)){
                        tokens.pb({I,buffer});
                        buffer="";
                    }
                    else if(!buffer.empty()){
                        lexemes.pb(buffer);
                        buffer="";
                    }
                }
                tokens.pb({P,curr});
            }
            else{
                buffer+=curr;
            }
        }
    }
    if(!buffer.empty() && !isstr)
    {
        if(chkconst(buffer)){
            tokens.pb({C,buffer});
            buffer="";
        }
        else if(chkident(buffer)){
            tokens.pb({I,buffer});
            buffer="";
        } 
        else if(!buffer.empty()){
            lexemes.pb(buffer);
            buffer="";
        }
        buffer="";
    }
}


void solve(){

    vector<pair<string,string>> tokens;
    vector<string> lexemes;

	std::ifstream file("test.c");
	if(!file){
		std::cerr<<"Error opening file!\n";
		exit(1);
	}


	std::stringstream buf;
	buf<<file.rdbuf();
	string line;


    /*
        #define I "Identifier"
        #define P "Punctuation"
        #define K "Keyword"
        #define C "Constant"
        #define S "String"
        #define O "Operator"
    */

    bool mcom=0;

	while(getline(buf,line)){
		// cout<<line<<endl;
        std::stringstream ss(line);
        string word;
        while(ss>>word){
            // cout<<word<<endl;

            //handle single and multi line comments
            if(mcom && word=="*/"){
                mcom=0;
                continue;
            }

            if(mcom) continue;

            if(word.size()>=2){
                if(word=="//"){
                    break;
                }
                if(word=="/*"){
                    mcom=1;
                    continue;
                }
            }

            //actual code
            if(keywords.find(word)!=keywords.end()){
                tokens.pb({K,word});
                continue;
            }
            if(operators.find(word)!=operators.end()){
                tokens.pb({O,word});
                continue;
            }
            if(punctuations.find(word)!=punctuations.end()){
                tokens.pb({P,word});
                continue;
            }
            if((word[0]=='\"' && word[(int)word.size()-1]=='\"') || (word[0]=='\'' && word[(int)word.size()-1]=='\'')){
               tokens.pb({S,word});
                continue;
            }
            //chk for identifiers, constants,string and invs
            chk(word,tokens,lexemes);
        }
        // cout<<"---------end---------"<<endl;
	}
    for(auto x:tokens){
        cout<<x.first<<" : "<<x.second<<endl;
    }
    cout<<endl<<endl<<"INVALID LEXEMES...\n"; 
    for(auto x:lexemes){
        cout<<x<<endl;
    }
	return;
}
int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    li tc = 1;
    while (tc--)
    {	
		solve();  
    }
    return 0;
}
