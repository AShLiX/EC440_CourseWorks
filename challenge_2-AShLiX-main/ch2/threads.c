#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/time.h>
#include <setjmp.h>

#include <unistd.h>
#include <errno.h>

#include "ec440threads.h"

/* You can support more threads. At least support this many. */
#define MAX_THREADS 128

/* Your stack should be this many bytes in size */
#define THREAD_STACK_SIZE 65535

/* Number of microseconds between scheduling events */
#define SCHEDULER_INTERVAL_USECS (50 * 1000)

/* Return value for undefined behaviors in mutex and barrier code in challenge_3 */
#define UNDEF_BEHAV_RET_CODE -1

/* Extracted from private libc headers. These are not part of the public
 * interface for jmp_buf.
 */
#define JB_RBX 0
#define JB_RBP 1
#define JB_R12 2
#define JB_R13 3
#define JB_R14 4
#define JB_R15 5
#define JB_RSP 6
#define JB_PC 7

/* thread_status identifies the current state of a thread. You can add, rename,
 * or delete these values. This is only a suggestion. */
enum thread_status
{
	TS_EXITED,
	TS_RUNNING,
	TS_READY
};

extern enum pstate_e get_status_phil(int p);





/* The thread control block stores information about a thread. You will
 * need one of this per thread.
 */
struct thread_control_block {
	
	/* TODO: add a thread ID */
	
	/* TODO: add information about its stack */
	unsigned long int* stack;
	
	/* TODO: add information about its registers */
	jmp_buf jump_buf;
	
	/* TODO: add information about the status (e.g., use enum thread_status) */
	enum thread_status status;
	
	/* Add other information you need to manage this thread */
	bool blocked;
};

// Global Variables
static struct thread_control_block *tcb_list;
static bool* thread_id_available;
static bool* thread_id_available_ECC;
static int current_running_thread;

// challenge_3 Struct Definitions
struct mutex_linkedList_element {

	pthread_mutex_t *mutex; // __align for searching.

	bool mutex_inUse;

	struct mutex_linkedList_element *next;
};

/*
struct barrier_linkedList_element {

	pthread_barrier_t barrier; // __align for searching.

	bool barrier_inUse;

	unsigned barrier_original_set_count;

	unsigned barrier_hits;

	int control_flow_int;

	bool barrier_destroyed;

	struct barrier_linkedList_element *next;
};
*/

// challenge_3 Global Variables
sigset_t new_set, old_set;

static bool globalVar_initialized;
static unsigned int rand_num_gen_seed;
static struct mutex_linkedList_element *mutex_list_first;
static int mutex_list_length;
//static struct barrier_linkedList_element *barrier_list_first;
//static int barrier_list_length;
static int **barrier_waiting_thread_array_array;
static int *barrier_waiting_thread_count_array;
static int *barrier_waiting_thread_original_set_count_array;
static bool *barrier_destroyed_array;
static int barrier_array_count;





// to supress compiler error
static void schedule(int signal) __attribute__((unused));

static void schedule(int signal)
{
	/* TODO: implement your round-robin scheduler 
	 * 1. Use setjmp() to update your currently-active thread's jmp_buf
	 *    You DON'T need to manually modify registers here.
	 * 2. Determine which is the next thread that should run
	 * 3. Switch to the next thread (use longjmp on that thread's jmp_buf)
	 */
	tcb_list[current_running_thread].status = TS_READY;
	int oldIndex = current_running_thread;
	int newIndex = current_running_thread;
	bool runnable_thread_NOT_found = true;
	for (int i = current_running_thread + 1; i < MAX_THREADS; i++)
	{
		if (thread_id_available[i] != thread_id_available_ECC[i])
		{
			if (tcb_list[i].blocked)
			{
				continue;
			}
			newIndex = i;
			runnable_thread_NOT_found = false;
			break;
		}
	}
	if (runnable_thread_NOT_found)
	{
		for (int i = 0; i < current_running_thread; i++)
		{
			if (thread_id_available[i] != thread_id_available_ECC[i])
			{
				if (tcb_list[i].blocked)
				{
					continue;
				}
				newIndex = i;
				runnable_thread_NOT_found = false;
				break;
			}
		}
	}
	current_running_thread = newIndex;
	tcb_list[current_running_thread].status = TS_RUNNING;
	if (!runnable_thread_NOT_found)
	{
		if (thread_id_available[oldIndex] != thread_id_available_ECC[oldIndex])
		{
			int retval = setjmp(tcb_list[oldIndex].jump_buf);
			if (!retval)
			{
				longjmp(tcb_list[current_running_thread].jump_buf, 0);
			}
		}
		else
		{
			longjmp(tcb_list[current_running_thread].jump_buf, 0);
		}
	}
}

static void mutex_barrier_globalVar_init()
{
	if (!globalVar_initialized)
	{
		// Global Data Structure Initialization for challenge_3
		globalVar_initialized = true;
		rand_num_gen_seed = 0;
		mutex_list_length = 0;
		//barrier_list_length = 0;
		barrier_array_count = 0;
	}
}

static void scheduler_init()
{
	/* TODO: do everything that is needed to initialize your scheduler. For example:
	 * - Allocate/initialize global threading data structures
	 * - Create a TCB for the main thread. Note: This is less complicated
	 *   than the TCBs you create for all other threads. In this case, your
	 *   current stack and registers are already exactly what they need to be!
	 *   Just make sure they are correctly referenced in your TCB.
	 * - Set up your timers to call schedule() at a 50 ms interval (SCHEDULER_INTERVAL_USECS)
	 */
	
	// Global Data Structure Initialization
	tcb_list = (struct thread_control_block*)malloc(sizeof(struct thread_control_block) * MAX_THREADS);
	thread_id_available = (bool*)malloc(sizeof(bool) * MAX_THREADS);
	thread_id_available_ECC = (bool*)malloc(sizeof(bool) * MAX_THREADS);
	for (int i = 1; i < MAX_THREADS; i++)
	{
		thread_id_available[i] = true;
		thread_id_available_ECC[i] = true;
		tcb_list[i].status = TS_EXITED;
	}

	// Create thread 0.
	// - We know thread id is 0 for the main thread so no need to find.
	thread_id_available[0] = false;
	thread_id_available_ECC[0] = true;
	current_running_thread = 0;
	
	// - Prepare tcb for main thread.
	tcb_list[0].status = TS_READY;
	tcb_list[0].blocked = false;

	mutex_barrier_globalVar_init();
	
	// Set up repeated timer.
	struct sigaction sigact;
	struct itimerval itmv;
	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_handler = &schedule;
	sigaction(SIGALRM, &sigact, NULL);
	itmv.it_value.tv_sec = 0;
	itmv.it_value.tv_usec = SCHEDULER_INTERVAL_USECS;
	itmv.it_interval.tv_sec = 0;
	itmv.it_interval.tv_usec = SCHEDULER_INTERVAL_USECS;
	setitimer(ITIMER_REAL, &itmv, NULL);
}

// Just found that we cannot make effective modification to %rsp with inline assembly.
// If we have to go this way, we need to write in a .S file.
/*
static void start_function(unsigned long int *stack, void *(*start_routine)(void *), void *arg)
{
	asm volatile(
	"movq %1, %%rax;"
	"movq %2, %%r12;"
	"movq %3, %%r13;"
	"movq %%rax, %%rsp;"
	"movq %%r13, %%rdi;"
	"jmpq *%r12;"
	:
	: "g" (stack), "g" (start_routine), "g" (arg)
	: "%rsp", "%rdi", "%rax", "%r12", "%r13"
	);
}
*/

int pthread_create(
	pthread_t *thread, const pthread_attr_t *attr,
	void *(*start_routine) (void *), void *arg)
{	
	/* TODO: Return 0 on successful thread creation, non-zero for an error.
	 *       Be sure to set *thread on success.
	 * Hints:
	 * The general purpose is to create a TCB:
	 * - Create a stack.
	 * - Assign the >stack pointer< in the thread's registers. Important: where
	 *   within the stack should the stack pointer be? It may help to draw
	 *   an empty stack diagram to answer that question.
	 * - Assign the >program counter< in the thread's registers.
	 * - Wait... HOW can you assign registers of that new stack?
	 *   1. call setjmp() to initialize a jmp_buf with your current thread
	 *   2. modify the internal data in that jmp_buf to create a new thread environment
	 *      env->__jmpbuf[JB_...] = ...
	 *      See the additional note about registers below
	 *   3. Later, when your scheduler runs, it will longjmp using your
	 *      modified thread environment, which will apply all the changes
	 *      you made here.
	 * - Remember to set your new thread as TS_READY, but only  after you
	 *   have initialized everything for the new thread.
	 * - Optionally: run your scheduler immediately (can also wait for the
	 *   next scheduling event).
	 */
	/*
	 * Setting registers for a new thread:
	 * When creating a new thread that will begin in start_routine, we
	 * also need to ensure that `arg` is passed to the start_routine.
	 * We cannot simply store `arg` in a register and set PC=start_routine.
	 * This is because the AMD64 calling convention keeps the first arg in
	 * the EDI register, which is not a register we control in jmp_buf.
	 * We provide a start_thunk function that copies R13 to RDI then jumps
	 * to R12, effectively calling function_at_R12(value_in_R13). So
	 * you can call your start routine with the given argument by setting
	 * your new thread's PC to be ptr_mangle(start_thunk), and properly
	 * assigning R12 and R13.
	 *
	 * Don't forget to assign RSP too! Functions know where to
	 * return after they finish based on the calling convention (AMD64 in
	 * our case). The address to return to after finishing start_routine
	 * should be the first thing you push on your stack.
	 */

	// Create the timer and handler for the scheduler. Create thread 0.
	static bool is_first_call = true;
	if (is_first_call)
	{
		is_first_call = false;
		scheduler_init();
	}

	// Create additional thread.
	// - Find available thread id.
	int tempId;
	bool available_thread_id_NOT_found = true;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (thread_id_available[i])
		{
			thread_id_available[i] = !thread_id_available[i];
			tempId = i;
			available_thread_id_NOT_found = false;
			break;
		}
	}
	if (available_thread_id_NOT_found)
	{
		return -1;
	}

	// - Prepare tcb for that thread.
	*thread = (pthread_t)tempId; // Change parameter effectively returns thread id.
	tcb_list[tempId].stack = (unsigned long int*)malloc(sizeof(unsigned long int) * THREAD_STACK_SIZE);
	tcb_list[tempId].stack[THREAD_STACK_SIZE - 8] = (unsigned long int)&pthread_exit;
	setjmp(tcb_list[tempId].jump_buf);
	tcb_list[tempId].jump_buf->__jmpbuf[JB_PC] = ptr_mangle((unsigned long int)start_thunk);
	tcb_list[tempId].jump_buf->__jmpbuf[JB_R12] = (long int)start_routine;
	tcb_list[tempId].jump_buf->__jmpbuf[JB_R13] = (long int)arg;
	tcb_list[tempId].jump_buf->__jmpbuf[JB_RSP] = ptr_mangle((unsigned long int)(tcb_list[tempId].stack + THREAD_STACK_SIZE - 8));
	tcb_list[tempId].status = TS_READY;
	tcb_list[tempId].blocked = false;
	
	return 0;
}

void pthread_exit(void *value_ptr)
{
	/* TODO: Exit the current thread instead of exiting the entire process.
	 * Hints:
	 * - Release all resources for the current thread. CAREFUL though.
	 *   If you free() the currently-in-use stack then do something like
	 *   call a function or add/remove variables from the stack, bad things
	 *   can happen.
	 * - Update the thread's status to indicate that it has exited
	 */
	tcb_list[current_running_thread].status = TS_EXITED;
	thread_id_available[current_running_thread] = !thread_id_available[current_running_thread];
	schedule(0);
	exit(0);
}

pthread_t pthread_self(void)
{
	/* TODO: Return the current thread instead of -1
	 * Hint: this function can be implemented in one line, by returning
	 * a specific variable instead of -1.
	 */
	return (pthread_t)current_running_thread;
}

/* Don't implement main in this file!
 * This is a library of functions, not an executable program. If you
 * want to run the functions in this file, create separate test programs
 * that have their own main functions.
 */

// ---v--- mutex code below ---v---

int pthread_mutex_init(
		pthread_mutex_t *restrict mutex,
		const pthread_mutexattr_t *restrict attr)
{
	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);
	
	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}

	// error on repeated initialization
	bool mutex_initialized = false;
	if (mutex_list_length)
	{
		if (mutex->__align == mutex_list_first->mutex->__align)
		{
			if (mutex->__size[0] == mutex_list_first->mutex->__size[0])
			{
				if (mutex->__size[1] == mutex_list_first->mutex->__size[1])
				{
					if (mutex->__size[2] == mutex_list_first->mutex->__size[2])
					{
						mutex_initialized = true;
					}
				}
			}
		}
		else
		{
			struct mutex_linkedList_element *current_mutex_element = mutex_list_first;
			for (int i = 1; i < mutex_list_length; i++)
			{
				current_mutex_element = current_mutex_element->next;
				if (mutex->__align == current_mutex_element->mutex->__align)
				{
					if (mutex->__size[0] == current_mutex_element->mutex->__size[0])
					{
						if (mutex->__size[1] == current_mutex_element->mutex->__size[1])
						{
							if (mutex->__size[2] == current_mutex_element->mutex->__size[2])
							{
								mutex_initialized = true;
								break;
							}
						}
					}
				}
			}
		}
	}
	if (mutex_initialized)
	{
		sigprocmask(SIG_SETMASK, &old_set, NULL);
		return UNDEF_BEHAV_RET_CODE;
	}

	// initialization
	struct mutex_linkedList_element *current_mutex_element =
		(struct mutex_linkedList_element *)malloc(sizeof(struct mutex_linkedList_element));
	current_mutex_element->mutex = mutex;
	current_mutex_element->mutex->__align = (long)mutex_list_length;
	srand(rand_num_gen_seed);
	rand_num_gen_seed += 2;
	current_mutex_element->mutex->__size[0] = rand() % 256 + '0';
	current_mutex_element->mutex->__size[1] = rand() % 256 + '0';
	current_mutex_element->mutex->__size[2] = rand() % 256 + '0';
	current_mutex_element->mutex_inUse = false;
	if (mutex_list_length)
	{
		current_mutex_element->next = mutex_list_first;
	}
	else
	{
		current_mutex_element->next = NULL;
	}
	mutex_list_first = current_mutex_element;
	mutex_list_length++;
	
	sigprocmask(SIG_SETMASK, &old_set, NULL);
	return 0;
}

int pthread_mutex_destroy(
		pthread_mutex_t *mutex)
{
	// error on destroying mutex in use
	// error on destroying uninitialized mutex
	// error on locking/unlocking destroyed mutex
	
	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);
	
	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}
	
	// Find mutex element in linked list.
	bool mutex_not_found = true;
	struct mutex_linkedList_element *current_mutex_element;
	struct mutex_linkedList_element *cache_mutex_element;
	bool is_first_element = false;
	bool is_last_element = false;
	int mutex_element_index;
	if (mutex_list_length)
	{
		if (mutex->__align == mutex_list_first->mutex->__align)
		{
			if (mutex->__size[0] == mutex_list_first->mutex->__size[0])
			{
				if (mutex->__size[1] == mutex_list_first->mutex->__size[1])
				{
					if (mutex->__size[2] == mutex_list_first->mutex->__size[2])
					{
						if (mutex_list_first->mutex_inUse)
						{
							sigprocmask(SIG_SETMASK, &old_set, NULL);
							return UNDEF_BEHAV_RET_CODE;
						}
						mutex_not_found = false;
						is_first_element = true;
						mutex_element_index = 0;
					}
				}
			}
		}
		else
		{
			current_mutex_element = mutex_list_first;
			for (int i = 1; i < mutex_list_length; i++)
			{
				cache_mutex_element = current_mutex_element;
				current_mutex_element = current_mutex_element->next;
				if (mutex->__align == current_mutex_element->mutex->__align)
				{
					if (mutex->__size[0] == current_mutex_element->mutex->__size[0])
					{
						if (mutex->__size[1] == current_mutex_element->mutex->__size[1])
						{
							if (mutex->__size[2] == current_mutex_element->mutex->__size[2])
							{
								if (current_mutex_element->mutex_inUse)
								{
									sigprocmask(SIG_SETMASK, &old_set, NULL);
									return UNDEF_BEHAV_RET_CODE;
								}
								mutex_not_found = false;
								if (i + 1 == mutex_list_length)
								{
									is_last_element = true;
								}
								mutex_element_index = i;
								break;
							}
						}
					}
				}
			}
		}
	}
	if (mutex_not_found)
	{
		sigprocmask(SIG_SETMASK, &old_set, NULL);
		return UNDEF_BEHAV_RET_CODE;
	}
	if (is_first_element)
	{
		struct mutex_linkedList_element *temp_mutex_element = mutex_list_first;
		mutex_list_first = mutex_list_first->next;
		free(temp_mutex_element);
	}
	else if (is_last_element)
	{
		cache_mutex_element->next = NULL;
		free(current_mutex_element);
		struct mutex_linkedList_element *temp_mutex_element = mutex_list_first;
		for (int i = 0; i < mutex_element_index; i++)
		{
			temp_mutex_element->mutex->__align--;
			temp_mutex_element = temp_mutex_element->next;
		}
	}
	else
	{
		cache_mutex_element->next = current_mutex_element->next;
		free(current_mutex_element);
		struct mutex_linkedList_element *temp_mutex_element = mutex_list_first;
		for (int i = 0; i < mutex_element_index; i++)
		{
			temp_mutex_element->mutex->__align--;
			temp_mutex_element = temp_mutex_element->next;
		}
	}
	mutex_list_length--;
	
	sigprocmask(SIG_SETMASK, &old_set, NULL);
	return 0;
}

int pthread_mutex_lock(
		pthread_mutex_t *mutex)
{
	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);

	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}
	
	// Find mutex element in linked list.
	bool mutex_not_found = true;
	if (mutex_list_length)
	{
		if (mutex->__align == mutex_list_first->mutex->__align)
		{
			if (mutex->__size[0] == mutex_list_first->mutex->__size[0])
			{
				if (mutex->__size[1] == mutex_list_first->mutex->__size[1])
				{
					if (mutex->__size[2] == mutex_list_first->mutex->__size[2])
					{
						while (mutex_list_first->mutex_inUse)
						{
							sigprocmask(SIG_SETMASK, &old_set, NULL);
							schedule(0);
						}
						mutex_list_first->mutex_inUse = true;
						mutex_not_found = false;
					}
				}
			}
		}
		else
		{
			struct mutex_linkedList_element *current_mutex_element = mutex_list_first;
			for (int i = 1; i < mutex_list_length; i++)
			{
				current_mutex_element = current_mutex_element->next;
				if (mutex->__align == current_mutex_element->mutex->__align)
				{
					if (mutex->__size[0] == current_mutex_element->mutex->__size[0])
					{
						if (mutex->__size[1] == current_mutex_element->mutex->__size[1])
						{
							if (mutex->__size[2] == current_mutex_element->mutex->__size[2])
							{
								while (current_mutex_element->mutex_inUse)
								{
									sigprocmask(SIG_SETMASK, &old_set, NULL);
									schedule(0);
								}
								current_mutex_element->mutex_inUse = true;
								mutex_not_found = false;
								break;
							}
						}
					}
				}
			}
		}
	}
	if (mutex_not_found)
	{
		sigprocmask(SIG_SETMASK, &old_set, NULL);
		return UNDEF_BEHAV_RET_CODE;
	}
	
	sigprocmask(SIG_SETMASK, &old_set, NULL);
	return 0;
}

int pthread_mutex_unlock(
		pthread_mutex_t *mutex)
{
	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);

	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}
	
	// Find mutex element in linked list.
	bool mutex_not_found = true;
	if (mutex_list_length)
	{
		if (mutex->__align == mutex_list_first->mutex->__align)
		{
			if (mutex->__size[0] == mutex_list_first->mutex->__size[0])
			{
				if (mutex->__size[1] == mutex_list_first->mutex->__size[1])
				{
					if (mutex->__size[2] == mutex_list_first->mutex->__size[2])
					{
						mutex_list_first->mutex_inUse = false;
						mutex_not_found = false;
					}
				}
			}
		}
		else
		{
			struct mutex_linkedList_element *current_mutex_element = mutex_list_first;
			for (int i = 1; i < mutex_list_length; i++)
			{
				current_mutex_element = current_mutex_element->next;
				if (mutex->__align == current_mutex_element->mutex->__align)
				{
					if (mutex->__size[0] == current_mutex_element->mutex->__size[0])
					{
						if (mutex->__size[1] == current_mutex_element->mutex->__size[1])
						{
							if (mutex->__size[2] == current_mutex_element->mutex->__size[2])
							{
								current_mutex_element->mutex_inUse = false;
								mutex_not_found = false;
								break;
							}
						}
					}
				}
			}
		}
	}
	if (mutex_not_found)
	{
		sigprocmask(SIG_SETMASK, &old_set, NULL);
		return UNDEF_BEHAV_RET_CODE;
	}
	
	sigprocmask(SIG_SETMASK, &old_set, NULL);
	return 0;
}

// ---v--- barrier code below ---v---
/*
int pthread_barrier_init(
		pthread_barrier_t *restrict barrier,
		const pthread_barrierattr_t *restrict attr,
		unsigned count)
{
	// error on count == 0, return EINVAL
	if (count == 0)
	{
		return EINVAL;
	}

	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);
	
	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}

	// error on repeated initialization
	bool barrier_initialized = false;
	if (barrier_list_length)
	{
		if (barrier->__align == barrier_list_first->barrier.__align)
		{
			if (barrier->__size[0] == barrier_list_first->barrier.__size[0])
			{
				if (barrier->__size[1] == barrier_list_first->barrier.__size[1])
				{
					if (barrier->__size[2] == barrier_list_first->barrier.__size[2])
					{
						if (barrier_list_first->barrier_destroyed)
						{
							barrier_list_first->barrier_inUse = false;
							barrier_list_first->barrier_original_set_count = count;
							barrier_list_first->barrier_hits = count;
							barrier_list_first->control_flow_int = 0;
							barrier_list_first->barrier_destroyed = false;
							sigprocmask(SIG_SETMASK, &old_set, NULL);
							return 0;
						}
						else
						{
							barrier_initialized = true;
						}
					}
				}
			}
		}
		else
		{
			struct barrier_linkedList_element *current_barrier_element = barrier_list_first;
			for (int i = 1; i < mutex_list_length; i++)
			{
				current_barrier_element = current_barrier_element->next;
				if (barrier->__align == current_barrier_element->barrier.__align)
				{
					if (barrier->__size[0] == current_barrier_element->barrier.__size[0])
					{
						if (barrier->__size[1] == current_barrier_element->barrier.__size[1])
						{
							if (barrier->__size[2] == current_barrier_element->barrier.__size[2])
							{
								if (current_barrier_element->barrier_destroyed)
								{
									barrier_list_first->barrier_inUse = false;
									barrier_list_first->barrier_original_set_count = count;
									barrier_list_first->barrier_hits = count;
									barrier_list_first->control_flow_int = 0;
									barrier_list_first->barrier_destroyed = false;
									sigprocmask(SIG_SETMASK, &old_set, NULL);
									return 0;
								}
								else
								{
									barrier_initialized = true;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	if (barrier_initialized)
	{
		sigprocmask(SIG_SETMASK, &old_set, NULL);
		return UNDEF_BEHAV_RET_CODE;
	}

	// initialization
	struct barrier_linkedList_element *current_barrier_element =
		(struct barrier_linkedList_element *)malloc(sizeof(struct barrier_linkedList_element));
	current_barrier_element->barrier.__align = (long)barrier_list_length;
	srand(rand_num_gen_seed);
	rand_num_gen_seed += 2;
	current_barrier_element->barrier.__size[0] = rand() % 256 + '0';
	current_barrier_element->barrier.__size[1] = rand() % 256 + '0';
	current_barrier_element->barrier.__size[2] = rand() % 256 + '0';
	barrier->__align = current_barrier_element->barrier.__align;
	barrier->__size[0] = current_barrier_element->barrier.__size[0];
	barrier->__size[1] = current_barrier_element->barrier.__size[1];
	barrier->__size[2] = current_barrier_element->barrier.__size[2];
	current_barrier_element->barrier_inUse = false;
	current_barrier_element->barrier_original_set_count = count;
	current_barrier_element->barrier_hits = count;
	current_barrier_element->control_flow_int = 0;
	current_barrier_element->barrier_destroyed = false;
	if (barrier_list_length)
	{
		current_barrier_element->next = barrier_list_first;
	}
	else
	{
		current_barrier_element->next = NULL;
	}
	barrier_list_first = current_barrier_element;
	barrier_list_length++;
	
	sigprocmask(SIG_SETMASK, &old_set, NULL);
	return 0;
}

int pthread_barrier_destroy(
		pthread_barrier_t *barrier)
{
	// error on destroying barrier in use
	// error on destroying uninitialized barrier
	// error on use destroyed barrier
	
	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);
	
	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}
	
	// Find barrier element in linked list.
	bool barrier_not_found = true;
	struct barrier_linkedList_element *current_barrier_element;
	//struct barrier_linkedList_element *cache_barrier_element;
	bool is_first_element = false;
	bool is_last_element = false;
	//int barrier_element_index;
	if (barrier_list_length)
	{
		if (barrier->__align == barrier_list_first->barrier.__align)
		{
			if (barrier->__size[0] == barrier_list_first->barrier.__size[0])
			{
				if (barrier->__size[1] == barrier_list_first->barrier.__size[1])
				{
					if (barrier->__size[2] == barrier_list_first->barrier.__size[2])
					{
						if (barrier_list_first->barrier_inUse)
						{
							sigprocmask(SIG_SETMASK, &old_set, NULL);
							return UNDEF_BEHAV_RET_CODE;
						}
						if (barrier_list_first->barrier_destroyed)
						{
							sigprocmask(SIG_SETMASK, &old_set, NULL);
							return UNDEF_BEHAV_RET_CODE;
						}
						barrier_not_found = false;
						is_first_element = true;
						//barrier_element_index = 0;
					}
				}
			}
		}
		else
		{
			current_barrier_element = barrier_list_first;
			for (int i = 1; i < barrier_list_length; i++)
			{
				//cache_barrier_element = current_barrier_element;
				current_barrier_element = current_barrier_element->next;
				if (barrier->__align == current_barrier_element->barrier.__align)
				{
					if (barrier->__size[0] == current_barrier_element->barrier.__size[0])
					{
						if (barrier->__size[1] == current_barrier_element->barrier.__size[1])
						{
							if (barrier->__size[2] == current_barrier_element->barrier.__size[2])
							{
								if (current_barrier_element->barrier_inUse)
								{
									sigprocmask(SIG_SETMASK, &old_set, NULL);
									return UNDEF_BEHAV_RET_CODE;
								}
								if (current_barrier_element->barrier_destroyed)
								{
									sigprocmask(SIG_SETMASK, &old_set, NULL);
									return UNDEF_BEHAV_RET_CODE;
								}
								barrier_not_found = false;
								if (i + 1 == barrier_list_length)
								{
									is_last_element = true;
								}
								//barrier_element_index = i;
								break;
							}
						}
					}
				}
			}
		}
	}
	if (barrier_not_found)
	{
		sigprocmask(SIG_SETMASK, &old_set, NULL);
		return UNDEF_BEHAV_RET_CODE;
	}
	if (is_first_element)
	{
		//struct barrier_linkedList_element *temp_barrier_element = barrier_list_first;
		//barrier_list_first = barrier_list_first->next;
		//free(temp_barrier_element);
		barrier_list_first->barrier_destroyed = true;
	}
	else if (is_last_element)
	{
		//cache_barrier_element->next = NULL;
		//free(current_barrier_element);
		//struct barrier_linkedList_element *temp_barrier_element = barrier_list_first;
		//for (int i = 0; i < barrier_element_index; i++)
		//{
		//	temp_barrier_element->barrier->__align--;
		//	temp_barrier_element = temp_barrier_element->next;
		//}
		current_barrier_element->barrier_destroyed = true;
	}
	else
	{
		//cache_barrier_element->next = current_barrier_element->next;
		//free(current_barrier_element);
		//struct barrier_linkedList_element *temp_barrier_element = barrier_list_first;
		//for (int i = 0; i < barrier_element_index; i++)
		//{
		//	temp_barrier_element->barrier->__align--;
		//	temp_barrier_element = temp_barrier_element->next;
		//}
		current_barrier_element->barrier_destroyed = true;
	}
	//barrier_list_length--;
	
	sigprocmask(SIG_SETMASK, &old_set, NULL);
	return 0;
}

int pthread_barrier_wait(
		pthread_barrier_t *barrier)
{
	bool barrier_not_found;
	bool is_before_goto = true;
	int control_flow_int;
	long cache_align = barrier->__align;
	char cache_size_0 = barrier->__size[0];
	char cache_size_1 = barrier->__size[1];
	char cache_size_2 = barrier->__size[2];
begin_of_wait:
	barrier_not_found = true;

	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);

	// one among all threads return PTHREAD_BARRIER_SERIAL_THREAD
	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}
	
	// Find barrier element in linked list.
	if (barrier_list_length)
	{
		if (cache_align == barrier_list_first->barrier.__align)
		{
			if (cache_size_0 == barrier_list_first->barrier.__size[0])
			{
				if (cache_size_1 == barrier_list_first->barrier.__size[1])
				{
					if (cache_size_2 == barrier_list_first->barrier.__size[2])
					{
						if (barrier_list_first->barrier_destroyed)
						{
							if (is_before_goto)
							{
								sigprocmask(SIG_SETMASK, &old_set, NULL);
								return UNDEF_BEHAV_RET_CODE;
							}
							else
							{
								sigprocmask(SIG_SETMASK, &old_set, NULL);
								return 0;
							}
						}
						else if (!is_before_goto)
						{
							if (control_flow_int == barrier_list_first->control_flow_int)
							{
								barrier_not_found = false;
							}
							else
							{
								sigprocmask(SIG_SETMASK, &old_set, NULL);
								return 0;
							}
						}
						else
						{
							barrier_list_first->barrier_inUse = true;
							barrier_list_first->barrier_hits--;
							if ((int)barrier_list_first->barrier_hits <= 0)
							{
								barrier_list_first->barrier_inUse = false;
								barrier_list_first->barrier_hits =
									barrier_list_first->barrier_original_set_count;
								barrier_list_first->control_flow_int++;
								sigprocmask(SIG_SETMASK, &old_set, NULL);
								return PTHREAD_BARRIER_SERIAL_THREAD;
							}
							barrier_not_found = false;
							control_flow_int = barrier_list_first->control_flow_int;
						}
					}
				}
			}
		}
		else
		{
			struct barrier_linkedList_element *current_barrier_element = barrier_list_first;
			for (int i = 1; i < barrier_list_length; i++)
			{
				current_barrier_element = current_barrier_element->next;
				if (cache_align == current_barrier_element->barrier.__align)
				{
					if (cache_size_0 == current_barrier_element->barrier.__size[0])
					{
						if (cache_size_1 == current_barrier_element->barrier.__size[1])
						{
							if (cache_size_2 == current_barrier_element->barrier.__size[2])
							{
								if (current_barrier_element->barrier_destroyed)
								{
									if (is_before_goto)
									{
										sigprocmask(SIG_SETMASK, &old_set, NULL);
										return UNDEF_BEHAV_RET_CODE;
									}
									else
									{
										sigprocmask(SIG_SETMASK, &old_set, NULL);
										return 0;
									}
								}
								else if (!is_before_goto)
								{
									if (control_flow_int == current_barrier_element->control_flow_int)
									{
										barrier_not_found = false;
										break;
									}
									else
									{
										sigprocmask(SIG_SETMASK, &old_set, NULL);
										return 0;
									}
								}
								else
								{
									current_barrier_element->barrier_inUse = true;
									current_barrier_element->barrier_hits--;
									if ((int)current_barrier_element->barrier_hits <= 0)
									{
										current_barrier_element->barrier_inUse = false;
										current_barrier_element->barrier_hits =
											current_barrier_element->barrier_original_set_count;
										current_barrier_element->control_flow_int++;
										sigprocmask(SIG_SETMASK, &old_set, NULL);
										return PTHREAD_BARRIER_SERIAL_THREAD;
									}
									barrier_not_found = false;
									control_flow_int = current_barrier_element->control_flow_int;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	if (barrier_not_found)
	{
		if (is_before_goto)
		{
			sigprocmask(SIG_SETMASK, &old_set, NULL);
			return UNDEF_BEHAV_RET_CODE;
		}
		else
		{
			sigprocmask(SIG_SETMASK, &old_set, NULL);
			return 0;
		}
	}

	is_before_goto = false;
	sigprocmask(SIG_SETMASK, &old_set, NULL);
	schedule(0);
	goto begin_of_wait;
	return 0;
}
*/
// ------v------barrier third redesign------v------

int pthread_barrier_init(
		pthread_barrier_t *restrict barrier,
		const pthread_barrierattr_t *restrict attr,
		unsigned count)
{
	// __align: key

	// error on count == 0, return EINVAL
	if (count == 0)
	{
		return EINVAL;
	}

	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);
	
	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}

	// error on repeated initialization
	int **new_bwtaa = (int**)malloc(sizeof(int*) * barrier_array_count + 1);
	int *new_bwtca = (int*)malloc(sizeof(int) * barrier_array_count + 1);
	int *new_bwtosca = (int*)malloc(sizeof(int) * barrier_array_count + 1);
	bool *new_bda = (bool*)malloc(sizeof(bool) * barrier_array_count + 1);
	for (int i = 0; i < barrier_array_count; i++)
	{
		new_bwtaa[i] = barrier_waiting_thread_array_array[i];
		new_bwtca[i] = barrier_waiting_thread_count_array[i];
		new_bwtosca[i] = barrier_waiting_thread_original_set_count_array[i];
		new_bda[i] = barrier_destroyed_array[i];
	}
	new_bwtaa[barrier_array_count] = (int*)malloc(sizeof(int) * count);
	new_bwtca[barrier_array_count] = 0;
	new_bwtosca[barrier_array_count] = count;
	new_bda[barrier_array_count] = false;
	barrier->__align = (long)barrier_array_count;
	free(barrier_waiting_thread_array_array);
	free(barrier_waiting_thread_count_array);
	free(barrier_waiting_thread_original_set_count_array);
	free(barrier_destroyed_array);
	barrier_waiting_thread_array_array = new_bwtaa;
	barrier_waiting_thread_count_array = new_bwtca;
	barrier_waiting_thread_original_set_count_array = new_bwtosca;
	barrier_destroyed_array = new_bda;
	barrier_array_count++;

	sigprocmask(SIG_SETMASK, &old_set, NULL);
	return 0;
}

int pthread_barrier_destroy(
		pthread_barrier_t *barrier)
{
	// error on destroying barrier in use
	// error on destroying uninitialized barrier
	// error on use destroyed barrier
	
	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);
	
	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}

	barrier_destroyed_array[(int)barrier->__align] = true;
	
	sigprocmask(SIG_SETMASK, &old_set, NULL);
	return 0;
}

int pthread_barrier_wait(
		pthread_barrier_t *barrier)
{
	// https://www.ibm.com/docs/en/i/7.1?topic=ssw_ibm_i_71/apis/sigpmsk.htm
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);

	// one among all threads return PTHREAD_BARRIER_SERIAL_THREAD
	static bool is_first_call = true;
	if (is_first_call)
	{
		mutex_barrier_globalVar_init();
	}

	if (barrier_destroyed_array[(int)barrier->__align])
	{
		sigprocmask(SIG_SETMASK, &old_set, NULL);
		return -1;
	}
	
	tcb_list[current_running_thread].blocked = true;
	barrier_waiting_thread_array_array
		[(int)barrier->__align]
		[barrier_waiting_thread_count_array[(int)barrier->__align]]
		= current_running_thread;
	barrier_waiting_thread_count_array[(int)barrier->__align]++;
	if (
		barrier_waiting_thread_count_array[(int)barrier->__align] ==
		barrier_waiting_thread_original_set_count_array[(int)barrier->__align]
	)
	{
		for (int i = 0; i < barrier_waiting_thread_count_array[(int)barrier->__align]; i++)
		{
			tcb_list[barrier_waiting_thread_array_array[(int)barrier->__align][i]].status = TS_READY;
			tcb_list[barrier_waiting_thread_array_array[(int)barrier->__align][i]].blocked = false;
		}
		barrier_waiting_thread_count_array[(int)barrier->__align] = 0;
		sigprocmask(SIG_SETMASK, &old_set, NULL);
		return PTHREAD_BARRIER_SERIAL_THREAD;
	}

	sigprocmask(SIG_SETMASK, &old_set, NULL);
	schedule(0);
	return 0;
}