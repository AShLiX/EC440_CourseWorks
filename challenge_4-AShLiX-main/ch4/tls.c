#include "tls.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

#define MAX_THREADS 128

/*
 * This is a good place to define any data structures you will use in this file.
 * For example:
 *  - struct TLS: may indicate information about a thread's local storage
 *    (which thread, how much storage, where is the storage in memory)
 *  - struct page: May indicate a shareable unit of memory (we specified in
 *    homework prompt that you don't need to offer fine-grain cloning and CoW,
 *    and that page granularity is sufficient). Relevant information for sharing
 *    could be: where is the shared page's data, and how many threads are sharing it
 *  - Some kind of data structure to help find a TLS, searching by thread ID.
 *    E.g., a list of thread IDs and their related TLS structs, or a hash table.
 */
struct page {
	int reference_count;
	char *data;
};

struct TLS {
	pthread_t threadID;
	unsigned int byte_count;
	int page_count;
	struct page **page_ptr_array;
};

/*
 * Now that data structures are defined, here's a good place to declare any
 * global variables.
 */
pthread_mutex_t dont_touch_my_mutex;

int page_size;
struct TLS *tls_array;
bool *tls_exist_array;

/*
 * With global data declared, this is a good point to start defining your
 * static helper functions.
 */

/*
 * Lastly, here is a good place to add your externally-callable functions.
 */
void sigsegv_handler(int signum, siginfo_t *info, void *ucontext)
{
	char *segmentation_fault_address = (char*)info->si_addr;
	
	char *page_begin_address;
	char *page_end_address;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (tls_exist_array[i])
		{
			for (int j = 0; j < tls_array[i].page_count; j++)
			{
				page_begin_address = tls_array[i].page_ptr_array[j]->data;
				page_end_address = page_begin_address + page_size;
				if (segmentation_fault_address >= page_begin_address)
				{
					if (segmentation_fault_address < page_end_address)
					{
						pthread_exit(0);
					}
				}
			}
		}
	}

	exit(SIGSEGV);
}

static void tls_init()
{
	static bool is_first_run = true;
	if (is_first_run)
	{
		pthread_mutex_init(&dont_touch_my_mutex, NULL);
		page_size = getpagesize();
		tls_array = (struct TLS*)calloc(MAX_THREADS, sizeof(struct TLS));
		tls_exist_array = (bool*)calloc(MAX_THREADS, sizeof(bool));
		for (int i = 0; i < MAX_THREADS; i++)
		{
			tls_exist_array[i] = false;
		}

		struct sigaction sigact;
		memset(&sigact, 0, sizeof(sigact));
		sigact.sa_flags = SA_SIGINFO;
		sigact.sa_sigaction = &sigsegv_handler;
		sigaction(SIGSEGV, &sigact, NULL);

		is_first_run = false;
	}
}





int tls_create(unsigned int size)
{
	static bool is_first_run = true;
	if (is_first_run)
	{
		tls_init();
		is_first_run = false;
	}

	//pthread_mutex_lock(&dont_touch_my_mutex);
	// error if thread already have more than 0 bytes of local storage
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (tls_exist_array[i])
		{
			if (tls_array[i].threadID == pthread_self())
			{
				return -1;
			}
		}
	}
	int tls_array_index;
	bool tls_array_position_not_found = true;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (!tls_exist_array[i])
		{
			tls_exist_array[i] = true;
			tls_array_index = i;
			tls_array_position_not_found = false;
			break;
		}
	}
	if (tls_array_position_not_found)
	{
		return -1;
	}
	tls_array[tls_array_index].threadID = pthread_self();
	tls_array[tls_array_index].byte_count = size;
	tls_array[tls_array_index].page_count = (int)size / page_size + 1;
	tls_array[tls_array_index].page_ptr_array = (struct page**)calloc
		(tls_array[tls_array_index].page_count, sizeof(struct page*));
	for (int i = 0; i < tls_array[tls_array_index].page_count; i++)
	{
		tls_array[tls_array_index].page_ptr_array[i] = (struct page*)malloc(sizeof(struct page));
		tls_array[tls_array_index].page_ptr_array[i]->reference_count = 1;
		// old memory claim method
		//tls_array[tls_array_index].page_ptr_array[i]->data = (char*)calloc(page_size, sizeof(char));
		// new memory claim method
		tls_array[tls_array_index].page_ptr_array[i]->data = (char*)mmap(NULL, page_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	}
	//pthread_mutex_unlock(&dont_touch_my_mutex);
	return 0;
}

int tls_destroy()
{
	static bool is_first_run = true;
	if (is_first_run)
	{
		tls_init();
		is_first_run = false;
	}

	//pthread_mutex_lock(&dont_touch_my_mutex);
	// error if the current thread has no LSA
	int tls_array_index;
	bool tls_array_position_not_found = true;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (tls_exist_array[i])
		{
			if (tls_array[i].threadID == pthread_self())
			{
				tls_array_index = i;
				tls_array_position_not_found = false;
				break;
			}
		}
	}
	if (tls_array_position_not_found)
	{
		return -1;
	}
	for (int i = 0; i < (int)tls_array[tls_array_index].page_count; i++)
	{
		tls_array[tls_array_index].page_ptr_array[i]->reference_count--;
		if (tls_array[tls_array_index].page_ptr_array[i]->reference_count <= 0)
		{
			// old memory free method
			//free(tls_array[tls_array_index].page_ptr_array[i]->data);
			// new memory free method
			munmap(tls_array[tls_array_index].page_ptr_array[i]->data, page_size);
			free(tls_array[tls_array_index].page_ptr_array[i]);
		}
	}
	free(tls_array[tls_array_index].page_ptr_array);
	tls_array[tls_array_index].page_count = 0;
	tls_array[tls_array_index].byte_count = (unsigned int)0;
	tls_exist_array[tls_array_index] = false;
	//pthread_mutex_unlock(&dont_touch_my_mutex);
	return 0;
}

int tls_read(unsigned int offset, unsigned int length, char *buffer)
{
	static bool is_first_run = true;
	if (is_first_run)
	{
		tls_init();
		is_first_run = false;
	}

	//pthread_mutex_lock(&dont_touch_my_mutex);
	// error if the current thread has no LSA
	int tls_array_index;
	bool tls_array_position_not_found = true;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (tls_exist_array[i])
		{
			if (tls_array[i].threadID == pthread_self())
			{
				tls_array_index = i;
				tls_array_position_not_found = false;
				break;
			}
		}
	}
	if (tls_array_position_not_found)
	{
		return -1;
	}
	// error if attempt to read more data than the LSA have
	if (offset + length > tls_array[tls_array_index].byte_count)
	{
		return -1;
	}
	// old single byte copy implementation
	// int page_index;
	// int in_page_index;
	// int previous_iteration_page_index = -1;
	// bool is_first_into_loop = true;
	// for (int i = offset; i < (int)(offset + length); i++)
	// {
	// 	page_index = i / page_size;
	// 	in_page_index = i % page_size;
	// 	if (page_index > previous_iteration_page_index)
	// 	{
	// 		if (is_first_into_loop)
	// 		{
	// 			is_first_into_loop = false;
	// 		}
	// 		else
	// 		{
	// 			if (mprotect(tls_array[tls_array_index].page_ptr_array[previous_iteration_page_index]->data, page_size, PROT_NONE))
	// 			{
	// 				return -1;
	// 			}
	// 		}
	// 		if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_READ))
	// 		{
	// 			return -1;
	// 		}
	// 		previous_iteration_page_index = page_index;
	// 	}
	// 	buffer[i - offset] = tls_array[tls_array_index].page_ptr_array[page_index]->data[in_page_index];
	// }
	// if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_NONE))
	// {
	// 	return -1;
	// }
	// new page_size copy implementation
	int page_index;
	int in_page_index;
	page_index = offset / page_size;
	in_page_index = offset % page_size;
	// copy section before fully aligned sections
	if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_READ))
	{
		return -1;
	}
	if (in_page_index + length > page_size)
	{
		memcpy(buffer, &tls_array[tls_array_index].page_ptr_array[page_index]->data[in_page_index], page_size - in_page_index);
	}
	else
	{
		memcpy(buffer, &tls_array[tls_array_index].page_ptr_array[page_index]->data[in_page_index], length);
	}
	if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_NONE))
	{
		return -1;
	}
	// copy fully aligned sections
	int aligned_offset = (page_index + 1) * page_size;
	for (int i = aligned_offset; i + page_size - 1 <= (int)(offset + length); i += page_size)
	{
		page_index = i / page_size;
		if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_READ))
		{
			return -1;
		}
		memcpy(&buffer[i - offset], tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size);
		if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_NONE))
		{
			return -1;
		}
	}
	// copy section after fully aligned sections
	if ((page_index + 1) * page_size < offset + length)
	{
		if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index + 1]->data, page_size, PROT_READ))
		{
			return -1;
		}
		memcpy(&buffer[(page_index + 1) * page_size - offset],
			tls_array[tls_array_index].page_ptr_array[page_index + 1]->data, offset + length - (page_index + 1) * page_size);
		if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index + 1]->data, page_size, PROT_NONE))
		{
			return -1;
		}
	}
	//pthread_mutex_unlock(&dont_touch_my_mutex);
	return 0;
}

int tls_write(unsigned int offset, unsigned int length, const char *buffer)
{
	static bool is_first_run = true;
	if (is_first_run)
	{
		tls_init();
		is_first_run = false;
	}

	//pthread_mutex_lock(&dont_touch_my_mutex);
	// error if the current thread has no LSA
	int tls_array_index;
	bool tls_array_position_not_found = true;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (tls_exist_array[i])
		{
			if (tls_array[i].threadID == pthread_self())
			{
				tls_array_index = i;
				tls_array_position_not_found = false;
				break;
			}
		}
	}
	if (tls_array_position_not_found)
	{
		return -1;
	}
	// error if attempt to write more data than the LSA can hold
	if (offset + length > tls_array[tls_array_index].byte_count)
	{
		return -1;
	}
	int page_index;
	int in_page_index;
	int previous_iteration_page_index = -1;
	bool is_first_into_loop = true;
	for (int i = offset; i < (int)(offset + length); i++)
	{
		page_index = i / page_size;
		in_page_index = i % page_size;
		if (page_index > previous_iteration_page_index)
		{
			if (is_first_into_loop)
			{
				is_first_into_loop = false;
			}
			else
			{
				if (mprotect(tls_array[tls_array_index].page_ptr_array[previous_iteration_page_index]->data, page_size, PROT_NONE))
				{
					return -1;
				}
			}
			if (tls_array[tls_array_index].page_ptr_array[page_index]->reference_count > 1)
			{
				struct page *tempPagePtr = (struct page*)malloc(sizeof(struct page));
				tempPagePtr->reference_count = 1;
				// old memory claim method
				//tempPagePtr->data = (char*)calloc(page_size, sizeof(char));
				// new memory claim method
				tempPagePtr->data = (char*)mmap(NULL, page_size, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
				if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_READ))
				{
					return -1;
				}
				for (int j = 0; j < page_size; j++)
				{
					tempPagePtr->data[j] = tls_array[tls_array_index].page_ptr_array[page_index]->data[j];
				}
				if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_NONE))
				{
					return -1;
				}
				tls_array[tls_array_index].page_ptr_array[page_index]->reference_count--;
				tls_array[tls_array_index].page_ptr_array[page_index] = tempPagePtr;
			}
			if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_WRITE))
			{
				return -1;
			}
			previous_iteration_page_index = page_index;
		}
		tls_array[tls_array_index].page_ptr_array[page_index]->data[in_page_index] = buffer[i - offset];
	}
	if (mprotect(tls_array[tls_array_index].page_ptr_array[page_index]->data, page_size, PROT_NONE))
	{
		return -1;
	}
	//pthread_mutex_unlock(&dont_touch_my_mutex);
	return 0;
}

int tls_clone(pthread_t tid)
{
	static bool is_first_run = true;
	if (is_first_run)
	{
		tls_init();
		is_first_run = false;
	}
	
	pthread_mutex_lock(&dont_touch_my_mutex);
	// error if the target thread has no LSA
	int target_tls_array_index;
	bool target_tls_array_position_not_found = true;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (tls_exist_array[i])
		{
			if (tls_array[i].threadID == tid)
			{
				target_tls_array_index = i;
				target_tls_array_position_not_found = false;
				break;
			}
		}
	}
	if (target_tls_array_position_not_found)
	{
		return -1;
	}
	// error if the current thread already has an LSA
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (tls_exist_array[i])
		{
			if (tls_array[i].threadID == pthread_self())
			{
				return -1;
			}
		}
	}
	int tls_array_index;
	bool tls_array_position_not_found = true;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (!tls_exist_array[i])
		{
			tls_exist_array[i] = true;
			tls_array_index = i;
			tls_array_position_not_found = false;
			break;
		}
	}
	if (tls_array_position_not_found)
	{
		return -1;
	}
	tls_array[tls_array_index].threadID = pthread_self();
	tls_array[tls_array_index].byte_count = tls_array[target_tls_array_index].byte_count;
	tls_array[tls_array_index].page_count = tls_array[target_tls_array_index].page_count;
	tls_array[tls_array_index].page_ptr_array = (struct page**)calloc
		(tls_array[tls_array_index].page_count, sizeof(struct page*));
	for (int i = 0; i < tls_array[tls_array_index].page_count; i++)
	{
		tls_array[tls_array_index].page_ptr_array[i] = tls_array[target_tls_array_index].page_ptr_array[i];
		tls_array[tls_array_index].page_ptr_array[i]->reference_count++;
	}
	pthread_mutex_unlock(&dont_touch_my_mutex);
	return 0;
}
