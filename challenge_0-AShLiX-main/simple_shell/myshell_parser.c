#include "myshell_parser.h"
#include "stddef.h"
#include "stdlib.h"

struct pipeline *pipeline_build(const char *command_line)
{
	struct pipeline* ppln = 0;
	if (command_line && command_line[0] != '\0' && command_line[0] != '\n')
	{
		ppln = (struct pipeline*) malloc(sizeof(struct pipeline));
		ppln->commands = (struct pipeline_command*) malloc(sizeof(struct pipeline_command));
		ppln->is_background = false;
		struct pipeline_command *last_command = ppln->commands;
		last_command->redirect_in_path = NULL;
		last_command->redirect_out_path = NULL;
		last_command->next = NULL;

		int current_index = 0;
		int begin_index = 0;
		int command_counter = 0;
		int redirect_mode = 0; //0 means normal, 1 means in(<), 2 means out(>)
		while (command_line[current_index] != '\0')
		{
			if (command_line[current_index] == ' ')
			{
				if (current_index != begin_index)
				{
					if (redirect_mode == 0)
					{
						if (command_counter >= MAX_ARGV_LENGTH)
						{
							abort();
						}
						last_command->command_args[command_counter] = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->command_args[command_counter][i - begin_index] = command_line[i];
						}
						last_command->command_args[command_counter][current_index - begin_index] = '\0';
						command_counter++;
					}
					else if (redirect_mode == 1)
					{
						last_command->redirect_in_path = (char*)malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->redirect_in_path[i - begin_index] = command_line[i];
						}
						last_command->redirect_in_path[current_index - begin_index] = '\0';
						redirect_mode = 0;
					}
					else if (redirect_mode == 2)
					{
						last_command->redirect_out_path = (char*)malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->redirect_out_path[i - begin_index] = command_line[i];
						}
						last_command->redirect_out_path[current_index - begin_index] = '\0';
						redirect_mode = 0;
					}
				}
				begin_index = current_index + 1;
			}
			else if (command_line[current_index] == '|')
			{
				if (current_index == 0)
				{
					begin_index = 1;
				}
				else
				{
					if (current_index != begin_index)
					{
						if (redirect_mode == 0)
						{
							if (command_counter >= MAX_ARGV_LENGTH)
							{
								abort();
							}
							last_command->command_args[command_counter] = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
							for (int i = begin_index; i < current_index; i++)
							{
								last_command->command_args[command_counter][i - begin_index] = command_line[i];
							}
							last_command->command_args[command_counter][current_index - begin_index] = '\0';
							command_counter++;
						}
						else if (redirect_mode == 1)
						{
							last_command->redirect_in_path = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
							for (int i = begin_index; i < current_index; i++)
							{
								last_command->redirect_in_path[i - begin_index] = command_line[i];
							}
							last_command->redirect_in_path[current_index - begin_index] = '\0';
							redirect_mode = 0;
						}
						else if (redirect_mode == 2)
						{
							last_command->redirect_out_path = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
							for (int i = begin_index; i < current_index; i++)
							{
								last_command->redirect_out_path[i - begin_index] = command_line[i];
							}
							last_command->redirect_out_path[current_index - begin_index] = '\0';
							redirect_mode = 0;
						}
					}
					for (int i = command_counter; i < MAX_ARGV_LENGTH; i++)
					{
						last_command->command_args[i] = NULL;
					}
					command_counter = 0;
					last_command->next = (struct pipeline_command*) malloc(sizeof(struct pipeline_command));
					last_command = last_command->next;
					last_command->redirect_in_path = NULL;
					last_command->redirect_out_path = NULL;
					last_command->next = NULL;
					begin_index = current_index + 1;
				}
			}
			else if (command_line[current_index] == '&')
			{
				if (current_index != begin_index)
				{
					if (redirect_mode == 0)
					{
						if (command_counter >= MAX_ARGV_LENGTH)
						{
							abort();
						}
						last_command->command_args[command_counter] = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->command_args[command_counter][i - begin_index] = command_line[i];
						}
						last_command->command_args[command_counter][current_index - begin_index] = '\0';
						command_counter++;
					}
					else if (redirect_mode == 1)
					{
						last_command->redirect_in_path = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->redirect_in_path[i - begin_index] = command_line[i];
						}
						last_command->redirect_in_path[current_index - begin_index] = '\0';
						redirect_mode = 0;
					}
					else if (redirect_mode == 2)
					{
						last_command->redirect_out_path = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->redirect_out_path[i - begin_index] = command_line[i];
						}
						last_command->redirect_out_path[current_index - begin_index] = '\0';
						redirect_mode = 0;
					}
				}
				ppln->is_background = true;
				begin_index = current_index + 1;
			}
			else if (command_line[current_index] == '<')
			{
				if (current_index != begin_index)
				{
					if (redirect_mode == 0)
					{
						if (command_counter >= MAX_ARGV_LENGTH)
						{
							abort();
						}
						last_command->command_args[command_counter] = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->command_args[command_counter][i - begin_index] = command_line[i];
						}
						last_command->command_args[command_counter][current_index - begin_index] = '\0';
						command_counter++;
					}
					else if (redirect_mode == 2)
					{
						last_command->redirect_out_path = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->redirect_out_path[i - begin_index] = command_line[i];
						}
						last_command->redirect_out_path[current_index - begin_index] = '\0';
						redirect_mode = 0;
					}
				}
				redirect_mode = 1;
				begin_index = current_index + 1;
			}
			else if (command_line[current_index] == '>')
			{
				if (current_index != begin_index)
				{
					if (redirect_mode == 0)
					{
						if (command_counter >= MAX_ARGV_LENGTH)
						{
							abort();
						}
						last_command->command_args[command_counter] = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->command_args[command_counter][i - begin_index] = command_line[i];
						}
						last_command->command_args[command_counter][current_index - begin_index] = '\0';
						command_counter++;
					}
					else if (redirect_mode == 1)
					{
						last_command->redirect_in_path = (char*) malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->redirect_in_path[i - begin_index] = command_line[i];
						}
						last_command->redirect_in_path[current_index - begin_index] = '\0';
						redirect_mode = 0;
					}
				}
				redirect_mode = 2;
				begin_index = current_index + 1;
			}
			else if (command_line[current_index] == '\n')
			{
				if (current_index != begin_index)
				{
					if (redirect_mode == 0)
					{
						if (command_counter >= MAX_ARGV_LENGTH)
						{
							abort();
						}
						last_command->command_args[command_counter] = (char*)malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->command_args[command_counter][i - begin_index] = command_line[i];
						}
						last_command->command_args[command_counter][current_index - begin_index] = '\0';
						command_counter++;
					}
					else if (redirect_mode == 1)
					{
						last_command->redirect_in_path = (char*)malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->redirect_in_path[i - begin_index] = command_line[i];
						}
						last_command->redirect_in_path[current_index - begin_index] = '\0';
						redirect_mode = 0;
					}
					else if (redirect_mode == 2)
					{
						last_command->redirect_out_path = (char*)malloc(sizeof(char) * (current_index - begin_index + 1));
						for (int i = begin_index; i < current_index; i++)
						{
							last_command->redirect_out_path[i - begin_index] = command_line[i];
						}
						last_command->redirect_out_path[current_index - begin_index] = '\0';
						redirect_mode = 0;
					}
				}
				for (int i = command_counter; i < MAX_ARGV_LENGTH; i++)
				{
					last_command->command_args[i] = NULL;
				}
				return ppln;
			}
			current_index++;
		}
	}
	return ppln;
}

void pipeline_free(struct pipeline *pipeline)
{
	if (pipeline)
	{
		if (pipeline->commands)
		{
			struct pipeline_command *last_command = pipeline->commands;
			struct pipeline_command *temp_command;
			while (last_command->next)
			{
				for (int i = 0; i < MAX_ARGV_LENGTH; i++)
				{
					free(last_command->command_args[i]);
				}
				temp_command = last_command;
				last_command = last_command->next;
				free(temp_command);
			}
			for (int i = 0; i < MAX_ARGV_LENGTH; i++)
			{
				free(last_command->command_args[i]);
			}
		}
	}
	return;
}
