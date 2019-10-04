#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define INPUT_MAX_SIZE    500
#define TOKEN_MAX_NUM     100
#define PATH_MAX_SIZE     200

int is_background_command(char *cmd_str)
{
	size_t cmd_len = strlen(cmd_str);
	
	while (cmd_len > 0)
	{
		if ((cmd_str[cmd_len - 1] == ' ') || (cmd_str[cmd_len - 1] == '\t'))
		{
			cmd_len--;
		}
		else
		{
			if (cmd_str[cmd_len - 1] == '&')
			{
				return 1;
			}
			cmd_len = 0;
		}
	}
	
	return 0;
}

int search_command(char *cmd_str, int chr)
{
	if (strchr(cmd_str, chr) == NULL)
	{
		return 0;
	}
	return 1;
}

/**
 * @brief Tokenize a C string 
 * 
 * @param str - The C string to tokenize 
 * @param delim - The C string containing delimiter character(s) 
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
size_t tokenize(char *str, const char *delim, char **argv)
{
    char *token;
    token = strtok(str, delim);
    size_t i = 0;
    for (i = 0; token != NULL; ++i)
    {
    	argv[i] = token;
        token = strtok(NULL, delim);
    }
    return i;
}

int main(int argc, char **argv)
{
    // print the string prompt without a newline, before beginning to read
    // tokenize the input, run the command(s), and print the result
    // do this in a loop
    
    char input_str[INPUT_MAX_SIZE];
    char *cmd_list[TOKEN_MAX_NUM];
    char *cmd_param[TOKEN_MAX_NUM];
    
    printf("Welcome to Dragon Shell!\n");
    
    while (1)
    {
        printf("dragonshell > ");
        
        if (fgets(input_str, INPUT_MAX_SIZE, stdin) != NULL) 
        {             
            size_t input_len = strlen(input_str);
            if (input_len > 1)
            {
                if (input_str[input_len - 1] == '\n')
                {
                    input_str[input_len - 1] = '\0';
                }
            }
        	
        	size_t cmd_num = tokenize(input_str, ";", cmd_list);
            
            for (size_t i = 0; i < cmd_num; i++)
            {          	
            	printf("%s\n", cmd_list[i]);
            	
            	char *cmd_str = cmd_list[i];
                
            	if (cmd_str != NULL)
                {
            		int background_flag = is_background_command(cmd_str);                   
                    int redirection_flag = search_command(cmd_str, '>');
                    int pipe_flag = search_command(cmd_str, '|');
                    
                    printf("%d %d %d\n", background_flag, redirection_flag, pipe_flag);
                    
                    size_t param_num = tokenize(cmd_str, " \t", cmd_param);
                    
                    
                }
            }
        }
    }

    return 0;
}
