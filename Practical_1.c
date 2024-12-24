#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Y printf("Valid string\n")
#define N printf("Invalid string\n")
void code()
{
    // a*bb
    char *input = NULL;
    size_t len = 0;
    size_t read;
    read = getline(&input, &len, stdin); // Read input dynamically
    if (read == -1)
    {
        printf("error reading input\n");
        exit(1);
    }
    if (input[read - 1] == '\n')
    {
        input[read - 1] = '\0'; // Remove newline character
        read--;
    }
    if (read == 0)
    {
        N;           // Invalid string if input is empty
        free(input); // Free the allocated memory
        return;
    }
    int fl = 0;
    for (int i = 0; i < read; i++)
    {
        if (input[i] == 'a' && fl)
        {
            N;           // Invalid string if 'a' follows 'b'
            free(input); // Free the allocated memory
            return;
        }
        if (input[i] == 'b')
        {
            fl++;
            if (fl > 2)
            {
                N;           // Invalid string if more than two 'b's
                free(input); // Free the allocated memory
                return;
            }
        }
    }
    Y;           // Valid string if it passes all checks
    free(input); // Free the allocated memory
}

int main()
{
    int tc = 1;
    scanf("%d", &tc); // Read the number of test cases
    getchar();        // Consume the newline character left in the input buffer
    while (tc--)
    {
        code(); // Execute the code function for each test case
    }
    printf("this code is performed by 22CE119-Keval\n");
    return 0;
}