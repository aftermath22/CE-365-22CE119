%option noyywrap
%{
    #include<stdio.h>
    int length = 0;
    int has_lower = 0;
    int has_upper = 0;
    int has_digit = 0;
    int has_symbol = 0;
%}




%%
([a-z]) { has_lower = 1; length++; }
([A-Z]) { has_upper = 1; length++; }
([0-9]) { has_digit = 1; length++; }
[*;#$@] { has_symbol = 1; length++; }
. { length++; }

\n {
    if (length < 9 || length > 15 || !has_lower || !has_upper || !has_digit || !has_symbol) {
        printf("Invalid password\n");
    } else {
        printf("Valid password\n");
    }
    length = 0;
    has_lower = 0;
    has_upper = 0;
    has_digit = 0;
    has_symbol = 0;
    return 0;
}



%%
int main(){
    printf("Enter the string: ");
    yylex();
    return 0;
}