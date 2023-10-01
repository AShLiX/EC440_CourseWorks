#include "myshell_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(x) do { \
	if (!(x)) { \
		fprintf(stderr, "%s:%d: Assertion (%s) failed!\n", \
				__FILE__, __LINE__, #x); \
	       	abort(); \
	} \
} while(0)

int
main(void)
{
	struct pipeline* my_pipeline = pipeline_build("ls\n");

	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
	TEST_ASSERT(!my_pipeline->is_background);
	TEST_ASSERT(my_pipeline->commands != NULL);

	// Test the parsed args
	TEST_ASSERT(strcmp("ls", my_pipeline->commands->command_args[0]) == 0);
	TEST_ASSERT(my_pipeline->commands->command_args[1] == NULL);

	// Test the redirect state
	TEST_ASSERT(my_pipeline->commands->redirect_in_path == NULL);
	TEST_ASSERT(my_pipeline->commands->redirect_out_path == NULL);

	// Test that there is only one parsed command in the pipeline
	TEST_ASSERT(my_pipeline->commands->next == NULL);

	pipeline_free(my_pipeline);
	/////////////////////////////////////////////////////////////
	my_pipeline = pipeline_build("/bin/cat my_file\n");

	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
	TEST_ASSERT(!my_pipeline->is_background);
	TEST_ASSERT(my_pipeline->commands != NULL);

	// Test the parsed args
	TEST_ASSERT(strcmp("/bin/cat", my_pipeline->commands->command_args[0]) == 0);
	TEST_ASSERT(strcmp("my_file", my_pipeline->commands->command_args[1]) == 0);
	TEST_ASSERT(my_pipeline->commands->command_args[2] == NULL);

	// Test the redirect state
	TEST_ASSERT(my_pipeline->commands->redirect_in_path == NULL);
	TEST_ASSERT(my_pipeline->commands->redirect_out_path == NULL);

	// Test that there is only one parsed command in the pipeline
	TEST_ASSERT(my_pipeline->commands->next == NULL);

	pipeline_free(my_pipeline);
	/////////////////////////////////////////////////////////////
	my_pipeline = pipeline_build("a& \n");

	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
	TEST_ASSERT(my_pipeline->is_background);
	TEST_ASSERT(my_pipeline->commands != NULL);

	// Test the parsed args
	TEST_ASSERT(strcmp("a", my_pipeline->commands->command_args[0]) == 0);
	TEST_ASSERT(my_pipeline->commands->command_args[1] == NULL);

	// Test the redirect state
	TEST_ASSERT(my_pipeline->commands->redirect_in_path == NULL);
	TEST_ASSERT(my_pipeline->commands->redirect_out_path == NULL);

	// Test that there is only one parsed command in the pipeline
	TEST_ASSERT(my_pipeline->commands->next == NULL);

	pipeline_free(my_pipeline);
	/////////////////////////////////////////////////////////////
	my_pipeline = pipeline_build("pipefrom| pipeto\n");

	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
	TEST_ASSERT(!my_pipeline->is_background);
	TEST_ASSERT(my_pipeline->commands != NULL);

	// Test the parsed args
	TEST_ASSERT(strcmp("pipefrom", my_pipeline->commands->command_args[0]) == 0);
	TEST_ASSERT(my_pipeline->commands->command_args[1] == NULL);

	// Test the redirect state
	TEST_ASSERT(my_pipeline->commands->redirect_in_path == NULL);
	TEST_ASSERT(my_pipeline->commands->redirect_out_path == NULL);

	// Test that there are two parsed commands in the pipeline
	TEST_ASSERT(my_pipeline->commands->next != NULL);
	struct pipeline_command* temp_command = my_pipeline->commands->next;
	TEST_ASSERT(temp_command != NULL);
	TEST_ASSERT(strcmp("pipeto", temp_command->command_args[0]) == 0);
	TEST_ASSERT(temp_command->command_args[1] == NULL);
	TEST_ASSERT(temp_command->redirect_in_path == NULL);
	TEST_ASSERT(temp_command->redirect_out_path == NULL);
	TEST_ASSERT(temp_command->next == NULL);

	pipeline_free(my_pipeline);
	/////////////////////////////////////////////////////////////
	my_pipeline = pipeline_build("cat< a_file\n");

	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
	TEST_ASSERT(!my_pipeline->is_background);
	TEST_ASSERT(my_pipeline->commands != NULL);

	// Test the parsed args
	TEST_ASSERT(strcmp("cat", my_pipeline->commands->command_args[0]) == 0);
	TEST_ASSERT(my_pipeline->commands->command_args[1] == NULL);

	// Test the redirect state
	TEST_ASSERT(strcmp("a_file", my_pipeline->commands->redirect_in_path) == 0);
	TEST_ASSERT(my_pipeline->commands->redirect_out_path == NULL);

	// Test that there is only one parsed command in the pipeline
	TEST_ASSERT(my_pipeline->commands->next == NULL);

	pipeline_free(my_pipeline);
	/////////////////////////////////////////////////////////////
	my_pipeline = pipeline_build("ls > txt\n");

	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
	TEST_ASSERT(!my_pipeline->is_background);
	TEST_ASSERT(my_pipeline->commands != NULL);

	// Test the parsed args
	TEST_ASSERT(strcmp("ls", my_pipeline->commands->command_args[0]) == 0);
	TEST_ASSERT(my_pipeline->commands->command_args[1] == NULL);

	// Test the redirect state
	TEST_ASSERT(my_pipeline->commands->redirect_in_path == NULL);
	TEST_ASSERT(strcmp("txt", my_pipeline->commands->redirect_out_path) == 0);

	// Test that there is only one parsed command in the pipeline
	TEST_ASSERT(my_pipeline->commands->next == NULL);

	pipeline_free(my_pipeline);
	/////////////////////////////////////////////////////////////
	my_pipeline = pipeline_build("chmod +x >txt< note +y |>txt bilibili&| dilidili\n");

	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
	TEST_ASSERT(my_pipeline->is_background);
	TEST_ASSERT(my_pipeline->commands != NULL);

	// Test the parsed args
	TEST_ASSERT(strcmp("chmod", my_pipeline->commands->command_args[0]) == 0);
	TEST_ASSERT(strcmp("+x", my_pipeline->commands->command_args[1]) == 0);
	TEST_ASSERT(strcmp("+y", my_pipeline->commands->command_args[2]) == 0);
	TEST_ASSERT(my_pipeline->commands->command_args[3] == NULL);

	// Test the redirect state
	TEST_ASSERT(strcmp("note", my_pipeline->commands->redirect_in_path) == 0);
	TEST_ASSERT(strcmp("txt", my_pipeline->commands->redirect_out_path) == 0);

	// Test that there are 3 parsed commands in the pipeline
	TEST_ASSERT(my_pipeline->commands->next != NULL);
	temp_command = my_pipeline->commands->next;
	TEST_ASSERT(strcmp("bilibili", temp_command->command_args[0]) == 0);
	TEST_ASSERT(temp_command->command_args[1] == NULL);
	TEST_ASSERT(temp_command->redirect_in_path == NULL);
	TEST_ASSERT(strcmp("txt", temp_command->redirect_out_path) == 0);
	TEST_ASSERT(temp_command->next != NULL);
	temp_command = temp_command->next;
	TEST_ASSERT(strcmp("dilidili", temp_command->command_args[0]) == 0);
	TEST_ASSERT(temp_command->command_args[1] == NULL);
	TEST_ASSERT(temp_command->redirect_in_path == NULL);
	TEST_ASSERT(temp_command->redirect_out_path == NULL);
	TEST_ASSERT(temp_command->next == NULL);

	pipeline_free(my_pipeline);
}
