%option noyywrap
%{
    #include<stdio.h>
    int charCount = 0;
    int wordCount = 0;
    int lineCount = 0;
%}

/* Rule section */

%%
([a-zA-Z0-9]+) {charCount += yyleng; wordCount++;}
([^a-zA-Z0-9\n]) {charCount++;}
\n {charCount++; lineCount++; return 0;}
%%

int main()
{
    printf("Enter the string: ");
    yylex(); 
    printf("Character:%d\nWords:%d\nLine:%d\n", charCount, wordCount, lineCount);
    return 0;
}