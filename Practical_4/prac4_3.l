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
\n {charCount++; lineCount++;}
<<EOF>> {return 0;} /* End counting only at EOF instead of newline */
%%
int main()
{
    printf("Enter text (Ctrl+D on Unix/Linux or Ctrl+Z on Windows to end input):\n");
    yylex(); 
    printf("Character:%d\nWords:%d\nLine:%d\n", charCount, wordCount, lineCount);
    return 0;
}
