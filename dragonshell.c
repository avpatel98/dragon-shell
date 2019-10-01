#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define INPUT_MAX_SIZE    500
#define TOKEN_MAX_NUM     100
#define PATH_MAX_SIZE     200

/**
 * @brief Tokenize a C string 
 * 
 * @param str - The C string to tokenize 
 * @param delim - The C string containing delimiter character(s) 
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
void tokenize(char *str, const char *delim, char **argv)
{
    char *token;
    token = strtok(str, delim);
    for (size_t i = 0; token != NULL; ++i)
    {
        argv[i] = token;
        token = strtok(NULL, delim);
    }
}

int main(int argc, char **argv)
{
    // print the string prompt without a newline, before beginning to read
    // tokenize the input, run the command(s), and print the result
    // do this in a loop
    
    char input_str[INPUT_MAX_SIZE];
    char *cmd_list[TOKEN_MAX_NUM];
    char *cmd_param[TOKEN_MAX_NUM];
    char curr_path[PATH_MAX_SIZE] = "/bin/:/usr/bin/";
    size_t input_len = 0;
    
    printf("Welcome to Dragon Shell!\n");
    
    while (1)
    {
        printf("dragonshell > ");
        
        if (fgets(input_str, INPUT_MAX_SIZE, stdin) != NULL) 
        {
            input_len = strlen(input_str);
            if (input_len > 1)
            {
                if (input_str[input_len - 1] == '\n')
                {
                    input_str[input_len - 1] = '\0';
                }
            }
                
            tokenize(cmd, ';', cmd_list);
            
            for (size_t i = 0; i < TKN_MAX_NUM; i++)
            {
                if (cmd_list[i] != NULL)
                {
                    if (strncmp(cmd_list[0], "$PATH",              
                }
                else
                {
                    break;
                }    
            }
        }
    }

    return 0;
}
