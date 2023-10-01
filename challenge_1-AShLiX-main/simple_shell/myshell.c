#include "myshell_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_INPUT_LENGTH 512
#define STDIN 0
#define STDOUT 1
#define STDERR 2

int main(int argc, char **argv)
{
	char user_input[MAX_INPUT_LENGTH];
	struct pipeline *ppln = NULL;
	struct pipeline_command *current_cmd = NULL;
	bool display_prompt = true;
	bool pipe_used;
	int *pipe_list[MAX_INPUT_LENGTH / 2];
	int pipe_current_index;
	for (int i = 0; i < MAX_INPUT_LENGTH / 2; i++)
	{
		pipe_list[i] = (int*)malloc(sizeof(int) * 2);
	}
	if (argc == 2 && (!strcmp(argv[1], "-n")))
	{
		display_prompt = false;
	}
	while (1)
	{
		pipe_used = false;
		pipe_current_index = 0;
		// Display Prompt
		if (display_prompt)
		{
			printf("my_shell$");
		}
		// Take User Input
		if (fgets(user_input, MAX_INPUT_LENGTH, stdin) == NULL)
		{
			break;
		}
		else
		{
			fflush(stdin);
			ppln = pipeline_build(user_input);
			if (ppln)
			{
				current_cmd = ppln->commands;
				pipe_used = false;
				if (ppln->is_background)
				{
					if (fork() > 0)
					{
						// Intentionally left empty
					}
					else
					{
						pid_t pid;
						int status;
						while (current_cmd->next)
						{
							if (pipe(pipe_list[pipe_current_index]) < 0)
							{
								perror("ERROR");
								for (int i = 0; i < MAX_INPUT_LENGTH / 2; i++)
								{
									free(pipe_list[i]);
								}
								pipeline_free(ppln);
								return 0;
							}
							if ((pid = fork()) > 0)
							{
								waitpid(pid, &status, 0);
								close(pipe_list[pipe_current_index][1]);
								pipe_used = true;
								pipe_current_index++;
								current_cmd = current_cmd->next;
							}
							else
							{
								// Changing Redirect In
								if (pipe_used)
								{
									close(pipe_list[pipe_current_index - 1][1]);
									close(STDIN);
									dup(pipe_list[pipe_current_index - 1][0]);
								}
								else if (current_cmd->redirect_in_path)
								{
									int tempFdNum = open(current_cmd->redirect_in_path, O_RDONLY);
									if (tempFdNum < 0)
									{
										perror("ERROR");
										exit(0);
									}
									close(STDIN);
									dup(tempFdNum);
								}
								// Changing Redirect Out
								close(pipe_list[pipe_current_index][0]);
								close(STDOUT);
								dup(pipe_list[pipe_current_index][1]);
								close(STDERR);
								dup(pipe_list[pipe_current_index][1]);
								int execvpRetCode = execvp(current_cmd->command_args[0], current_cmd->command_args);
								if (execvpRetCode)
								{
									perror("ERROR");
									exit(0);
								}
							}
						}
						if ((pid = fork()) > 0)
						{
							waitpid(pid, &status, 0);
							exit(0);
						}
						else
						{
							// Changing Redirect In
							if (pipe_used)
							{
								close(pipe_list[pipe_current_index - 1][1]);
								close(STDIN);
								dup(pipe_list[pipe_current_index - 1][0]);
							}
							else if (current_cmd->redirect_in_path)
							{
								int tempFdNum = open(current_cmd->redirect_in_path, O_RDONLY);
								if (tempFdNum < 0)
								{
									perror("ERROR");
									exit(0);
								}
								close(STDIN);
								dup(tempFdNum);
							}
							// Changing Redirect Out
							if (current_cmd->redirect_out_path)
							{
								int tempFdNum = creat(current_cmd->redirect_out_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
								close(STDOUT);
								dup(tempFdNum);
								close(STDERR);
								dup(tempFdNum);
							}
							int execvpRetCode = execvp(current_cmd->command_args[0], current_cmd->command_args);
							if (execvpRetCode)
							{
								perror("ERROR");
								exit(0);
							}
						}
					}
				}
				else
				{
					pid_t pid;
					int status;
					while (current_cmd->next)
					{
						if (pipe(pipe_list[pipe_current_index]) < 0)
						{
							perror("ERROR");
							for (int i = 0; i < MAX_INPUT_LENGTH / 2; i++)
							{
								free(pipe_list[i]);
							}
							pipeline_free(ppln);
							return 0;
						}
						if ((pid = fork()) > 0)
						{
							waitpid(pid, &status, 0);
							close(pipe_list[pipe_current_index][1]);
							pipe_used = true;
							pipe_current_index++;
							current_cmd = current_cmd->next;
						}
						else
						{
							// Changing Redirect In
							if (pipe_used)
							{
								close(pipe_list[pipe_current_index - 1][1]);
								close(STDIN);
								dup(pipe_list[pipe_current_index - 1][0]);
							}
							else if (current_cmd->redirect_in_path)
							{
								int tempFdNum = open(current_cmd->redirect_in_path, O_RDONLY);
								if (tempFdNum < 0)
								{
									perror("ERROR");
									exit(0);
								}
								close(STDIN);
								dup(tempFdNum);
							}
							// Changing Redirect Out
							close(pipe_list[pipe_current_index][0]);
							close(STDOUT);
							dup(pipe_list[pipe_current_index][1]);
							close(STDERR);
							dup(pipe_list[pipe_current_index][1]);
							int execvpRetCode = execvp(current_cmd->command_args[0], current_cmd->command_args);
							if (execvpRetCode)
							{
								perror("ERROR");
								exit(0);
							}
						}
					}
					if ((pid = fork()) > 0)
					{
						waitpid(pid, &status, 0);
					}
					else
					{
						// Changing Redirect In
						if (pipe_used)
						{
							close(pipe_list[pipe_current_index - 1][1]);
							close(STDIN);
							dup(pipe_list[pipe_current_index - 1][0]);
						}
						else if (current_cmd->redirect_in_path)
						{
							int tempFdNum = open(current_cmd->redirect_in_path, O_RDONLY);
							if (tempFdNum < 0)
							{
								perror("ERROR");
								exit(0);
							}
							close(STDIN);
							dup(tempFdNum);
						}
						// Changing Redirect Out
						if (current_cmd->redirect_out_path)
						{
							int tempFdNum = creat(current_cmd->redirect_out_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
							close(STDOUT);
							dup(tempFdNum);
							close(STDERR);
							dup(tempFdNum);
						}
						int execvpRetCode = execvp(current_cmd->command_args[0], current_cmd->command_args);
						if (execvpRetCode)
						{
							perror("ERROR");
							exit(0);
						}
					}
				}
				pipeline_free(ppln);
			}
		}
	}
	for (int i = 0; i < MAX_INPUT_LENGTH / 2; i++)
	{
		free(pipe_list[i]);
	}
	return 0;
}
