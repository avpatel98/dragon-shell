#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define INPUT_MAX_SIZE		500
#define TOKEN_MAX_NUM		100
#define PATH_MAX_SIZE		200
#define PATH_MAX_NUM		50

char curr_path[PATH_MAX_NUM][PATH_MAX_SIZE];
size_t curr_path_num = 0;

int background_cpid = -1;

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

void execute_cd_command(char **cmd_param)
{
	if (cmd_param[1] == NULL)
	{
		printf("dragonshell: expected argument to \"cd\"\n");
	}
	else if (chdir(cmd_param[1]) == -1)
	{
		printf("dragonshell: No such file or directory\n");
	}
}

void execute_pwd_command(char **cmd_param)
{
	char curr_direct[PATH_MAX_SIZE];
	
	if (getcwd(curr_direct, PATH_MAX_SIZE) != NULL)
	{
		printf("%s\n", curr_direct);
	}
	else
	{
		perror("getcwd");
	}
}

void execute_print_path_command(void)
{
	printf("Current PATH: ");
	for (size_t path_ind = 0; path_ind < curr_path_num; path_ind++)
	{
		printf("%s", curr_path[path_ind]);
		if (path_ind < (curr_path_num - 1))
		{
			printf(":");
		}
	}
	printf("\n");
}

void execute_a2path_command(char **cmd_param)
{
	if (cmd_param[1] == NULL)
	{
		curr_path_num = 0;
	}
	else 
	{
		char *directories[PATH_MAX_NUM] = {NULL};
		tokenize(cmd_param[1], ":", directories);
		
		size_t direct_ind = 0;
		if (directories[0] != NULL)
		{
			if (strcmp(directories[0], "$PATH") == 0)
			{
				direct_ind = 1;
			}
			else
			{
				curr_path_num = 0;
			}
		}
		
		while (directories[direct_ind] != NULL)
		{
			strcpy(curr_path[curr_path_num++], directories[direct_ind++]);
		}
	}
}

void execute_exit_command(void)
{
	printf("Exiting\n");
	_exit(0);
}

int main(int argc, char **argv)
{
    // print the string prompt without a newline, before beginning to read
    // tokenize the input, run the command(s), and print the result
    // do this in a loop
    
    char input_str[INPUT_MAX_SIZE];  
    
    printf("Welcome to Dragon Shell!\n");
    
    strcpy(curr_path[0], "/bin/");
    strcpy(curr_path[1], "/usr/bin/");
    curr_path_num = 2;
    
    while (1)
    {
        printf("dragonshell > ");
        fflush(stdout);
        
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
        	
            char *cmd_list[TOKEN_MAX_NUM] = {NULL};
        	tokenize(input_str, ";", cmd_list);
        	
        	size_t cmd_ind = 0;
            while (cmd_list[cmd_ind] != NULL)
            { 
				printf("%s\n", cmd_list[cmd_ind]);
					
				char *cmd_str = cmd_list[cmd_ind++];
					
				int background_flag = is_background_command(cmd_str);                   
				int redirection_flag = search_command(cmd_str, '>');
				int pipe_flag = search_command(cmd_str, '|');
				
				printf("%d %d %d\n", background_flag, redirection_flag, pipe_flag);
				
				char *cmd_param[TOKEN_MAX_NUM] = {NULL};
				tokenize(cmd_str, " \t", cmd_param);
				
				if (cmd_param[0] != NULL)
				{
					if (strcmp(cmd_param[0], "cd") == 0)
					{
						execute_cd_command(cmd_param);
					}
					else if (strcmp(cmd_param[0], "pwd") == 0)
					{
						execute_pwd_command(cmd_param);
					}
					else if (strcmp(cmd_param[0], "$PATH") == 0)
					{
						execute_print_path_command();
					}
					else if (strcmp(cmd_param[0], "a2path") == 0)
					{
						execute_a2path_command(cmd_param);
					}
					else if (strcmp(cmd_param[0], "exit") == 0)
					{
						execute_exit_command();
					}
					else
					{
						pid_t pid = fork();
						
						if (pid < 0)
						{
							perror("fork");
						}
						else if (pid == 0)
						{
							
						}
						else
						{
							if (background_flag)
							{
								background_cpid = pid;
								printf("PID %d is running in the background\n", background_cpid);
							}
							else
							{
								waitpid(pid, NULL, 0);
							}
						}
					}
				}
            }
        }
        else
        {
        	printf("\n");
        	execute_exit_command();
        }
    }

    return 0;
}
