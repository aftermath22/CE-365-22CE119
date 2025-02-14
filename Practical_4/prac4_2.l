%option noyywrap
%{
    #include<stdio.h>
%}

/* Rule section */  

%%
([cC][hH][aA][rR][uU][sS][aA][tT]) {printf("university");}
. {printf("%s", yytext);}
%%
int main()
{
    printf("Enter the string: ");
    yylex();  // call rule section

    return 0;    
}