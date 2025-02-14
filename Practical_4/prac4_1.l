%option noyywrap
%{
    #include<stdio.h>
%}

/* Rule section */
%%
([a-zA-Z]+) {}
([^0-9\n]) {}
([0-9]+) {printf("%d\n", atoi(yytext));}
\n 
%%
int main()
{
    printf("Enter the string: ");
    yylex();  // call rule section

    return 0;
}