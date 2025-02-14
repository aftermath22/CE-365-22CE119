%{
#include <stdio.h>
#include <stdlib.h>
%}

%option noyywrap

%x COMMENT STRING_DQ STRING_SQ

DIGIT     [0-9]
ID        [a-zA-Z_][a-zA-Z0-9_]*
KEYWORD   "int"|"char"|"return"|"if"|"else"|"while"|"for"|"do"|"float"|"double"|"void"|"short"|"long"|"signed"|"unsigned"|"struct"|"union"|"enum"|"typedef"|"auto"|"register"|"static"|"extern"|"break"|"case"|"continue"|"default"|"goto"|"sizeof"|"volatile"|"const"|"exit"
OPERATOR  "+"|"-"|"*"|"/"|"="|"=="|"!="|"<="|">="|"++"|"--"|"+="|"-="|"*="|"/="|"%"|"<"|">"|"&&"|"||"|"!"|"&"|"|"|"^"|"~"|"<<"|">>"|"->"|"."
PUNCT     [(){},;:\[\]]
SCOMMENT  "//".*
WS        [ \t\n]

INT_CONST      {DIGIT}+
FLOAT_CONST    {DIGIT}+\.({DIGIT}+)?([eE][-+]?{DIGIT}+)?[fF]?
EXP_CONST      {DIGIT}+[eE][-+]?{DIGIT}+
CHAR_CONST     '\\?.'
STRING_CONST   \"([^\\\"]|\\.)*\"

%%

{KEYWORD}      { printf("Keyword: %s\n", yytext); }
{OPERATOR}     { printf("Operator: %s\n", yytext); }
{PUNCT}        { printf("Punctuation: %s\n", yytext); }
{FLOAT_CONST}  { printf("Floating Constant: %s\n", yytext); }
{EXP_CONST}    { printf("Exponential Constant: %s\n", yytext); }
{INT_CONST}    { printf("Integer Constant: %s\n", yytext); }
{CHAR_CONST}   { printf("Character: %s\n", yytext); }
{STRING_CONST} { printf("String: %s\n", yytext); }
{ID}           { printf("Identifier: %s\n", yytext); }
{WS}           { /* Ignore whitespace */ }
{SCOMMENT}     { /* Ignore single-line comment */ }

"/*"         { BEGIN(COMMENT); }

<COMMENT>{
  "*/"       { BEGIN(INITIAL); }  // Exit comment mode properly
  [^*]+      { }  // Consume everything except '*'
  "*"+[^/]   { }  // Handle multiple '*' but not "*/"
  .          { }  // Consume everything else (including newlines)
  <<EOF>>    { printf("Error: Unterminated comment\n"); yyterminate(); }
}



"\""         { BEGIN(STRING_DQ); yytext[0] = ' '; }
<STRING_DQ>{
  "\""       { BEGIN(INITIAL); printf("String: %s\n", yytext); }
  \\.        { }
  [^\\"\n]+  { }
  \n         { printf("Error: Unterminated string\n"); BEGIN(INITIAL); }
}
"'"          { BEGIN(STRING_SQ); yytext[0] = ' '; }
<STRING_SQ>{
  "'"        { BEGIN(INITIAL); printf("Character: %s\n", yytext); }
  \\.        { }
  [^\\'\n]+  { }
  \n         { printf("Error: Unterminated character constant\n"); BEGIN(INITIAL); }
}
.              { printf("Error: Invalid token '%s'\n", yytext); }

%%

int main(int argc, char **argv) {
    yylex();
    return 0;
}

