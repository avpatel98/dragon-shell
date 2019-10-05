#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define INPUT_MAX_SIZE		500
#define TOKEN_MAX_NUM		100
#define PATH_MAX_SIZE		200
#define PATH_MAX_NUM		50

char curr_path[PATH_MAX_NUM][PATH_MAX_SIZE];
size_t curr_path_num = 0;

pid_t background_cpid = -1;
pid_t foreground_cpid = -1;

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

void signal_forward_handler(int signum)
{
	if (background_cpid > 0)
	{
		kill(background_cpid, signum);
	}
	if (foreground_cpid > 0)
	{
		kill(foreground_cpid, signum);
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
				cmd_str[cmd_len - 1] = '\0';
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
		perror("getcwd error");
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
		char *direct_paths[PATH_MAX_NUM] = {NULL};
		tokenize(cmd_param[1], ":", direct_paths);
		
		size_t direct_ind = 0;
		if (direct_paths[0] != NULL)
		{
			if (strcmp(direct_paths[0], "$PATH") == 0)
			{
				direct_ind = 1;
			}
			else
			{
				curr_path_num = 0;
			}
		}
		
		while (direct_paths[direct_ind] != NULL)
		{
			strcpy(curr_path[curr_path_num++], direct_paths[direct_ind++]);
		}
	}
}

void execute_exit_command(void)
{
	printf("Exiting\n");
	
	if (background_cpid > 0)
	{
		kill(background_cpid, SIGKILL);
	}
	
	while (wait(NULL) > 0);
	
	_exit(0);
}

void initiate_redirection(char *cmd_str)
{
	char *redirect_param[5] = {NULL};
	tokenize(cmd_str, ">", redirect_param);
	cmd_str = redirect_param[0];
	
	if (redirect_param[1] != NULL)
	{
		char *file_name[5] = {NULL};
		tokenize(redirect_param[1], " \t", file_name);
		
		int file_desc = open(file_name[0], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
		if (file_desc == -1)
		{
			perror("open error");
		}
		
		dup2(file_desc, STDOUT_FILENO);
		close(file_desc);
	}
}

void execute_command(char **cmd_param)
{
	char *env[] = {NULL};
	
	if (strncmp(cmd_param[0], "/", 1) == 0)
	{
		execve(cmd_param[0], cmd_param, env);
	}
	else
	{
		char direct_path[PATH_MAX_SIZE];
		
		if (getcwd(direct_path, PATH_MAX_SIZE) != NULL)
		{		
			strcat(direct_path, "/");
			strcat(direct_path, cmd_param[0]);
			execve(direct_path, cmd_param, env);
		}
		
		for (size_t direct_ind = 0; direct_ind < curr_path_num; direct_ind++)
		{	
			strcpy(direct_path, curr_path[direct_ind]);
			strcat(direct_path, cmd_param[0]);
			execve(direct_path, cmd_param, env);
		}
	}
}

int main(int argc, char **argv)
{
    // print the string prompt without a newline, before beginning to read
    // tokenize the input, run the command(s), and print the result
    // do this in a loop
    
    char input_str[INPUT_MAX_SIZE];
    
    signal(SIGINT, signal_forward_handler);
    signal(SIGTSTP, signal_forward_handler);
    
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
            if (input_len > 0)
            {
                if (input_str[input_len - 1] == '\n')
                {
                    if (input_len == 1)
                    {
                    	continue;
                    }
                    else
                    {
                    	input_str[input_len - 1] = '\0';
                    }
                }
            }
        	
            char *cmd_list[TOKEN_MAX_NUM] = {NULL};
        	tokenize(input_str, ";", cmd_list);
        	
        	size_t cmd_ind = 0;
            while (cmd_list[cmd_ind] != NULL)
            {
				char *cmd_str = cmd_list[cmd_ind++];
					
				int background_flag = is_background_command(cmd_str);
				int pipe_flag = search_command(cmd_str, '|');
				
				char *cmd_param[TOKEN_MAX_NUM] = {NULL};
				
				char *pipe_param[5] = {NULL};
				if (pipe_flag)
				{
					tokenize(cmd_str, "|", pipe_param);
					cmd_str = pipe_param[0];
				}
				
				int redirect_flag = search_command(cmd_str, '>');
				int stdout_desc = dup(STDOUT_FILENO);
				
				if (redirect_flag)
				{
					initiate_redirection(cmd_str);
				}
				
				tokenize(cmd_str, " \t", cmd_param);
				
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
					if (pid == -1)
					{
						perror("fork error");
					}
					else if (pid == 0)
					{
						signal(SIGINT, SIG_DFL);
						signal(SIGTSTP, SIG_DFL);
						
						if (background_flag)
						{
							close(STDOUT_FILENO);
							close(STDERR_FILENO);
						}
						
						if (pipe_flag)
						{
							int file_descs[2];
							if (pipe(file_descs) == -1)
							{
								perror("pipe error");
							}
							
							pid_t cpid = fork();
							if (cpid == -1)
							{
								perror("fork error");
							}
							else if (cpid == 0)
							{
								close(file_descs[0]);
								
								if (!redirect_flag)
								{
									dup2(file_descs[1], STDOUT_FILENO);
									close(file_descs[1]);
								}
								
								execute_command(cmd_param);
								
								if (!redirect_flag)
								{
									fflush(stdout);
									dup2(stdout_desc, STDOUT_FILENO);
									close(stdout_desc);
								}
							}
							else
							{
								close(file_descs[1]);
								dup2(file_descs[0], STDIN_FILENO);
								close(file_descs[0]);
								
								if (pipe_param[1] != NULL)
								{
									cmd_str = pipe_param[1];
									
									redirect_flag = search_command(cmd_str, '>');
									if (redirect_flag)
									{
										initiate_redirection(cmd_str);
									}
									
									char *cmd2_param[TOKEN_MAX_NUM] = {NULL};
									tokenize(cmd_str, " \t", cmd2_param);
									
									execute_command(cmd2_param);
								}
							}
						}
						else
						{
							execute_command(cmd_param);
						}
						
						printf("dragonshell: %s: command not found\n", cmd_param[0]);
						
						_exit(1);
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
							foreground_cpid = pid;
							waitpid(foreground_cpid, NULL, 0);
							foreground_cpid = -1;
						}
					}
				}
				
				if (redirect_flag)
				{
					fflush(stdout);
					dup2(stdout_desc, STDOUT_FILENO);
					close(stdout_desc);
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
