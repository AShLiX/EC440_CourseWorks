override CFLAGS := -Wall -Werror -std=gnu99 -O0 -g -U_FORTIFY_SOURCE $(CFLAGS) -I.

# Build the threads.o file
threads.o: threads.c ec440threads.h

test_busy_threads.o: test_busy_threads.c ec440threads.h

test_busy_threads: test_busy_threads.o threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@

lock_test_1.o: lock_test_1.c ec440threads.h

lock_test_1: lock_test_1.o threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@

barrier_test_1.o: barrier_test_1.c ec440threads.h

barrier_test_1: barrier_test_1.o threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@

gs_test_1.o: gs_test_1.c ec440threads.h

gs_test_1: gs_test_1.o threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@

gs_test_2.o: gs_test_2.c ec440threads.h

gs_test_2: gs_test_2.o threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@

test_parallel_and_destroy_reuse.o: test_parallel_and_destroy_reuse.c ec440threads.h

test_parallel_and_destroy_reuse: test_parallel_and_destroy_reuse.o threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@

test_destroy_reuse.o: test_destroy_reuse.c ec440threads.h

test_destroy_reuse: test_destroy_reuse.o threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@


test_files=test_busy_threads

lock_test_files=lock_test_1

barrier_test_files=barrier_test_1

gs_test_1_files=gs_test_1

gs_test_2_files=gs_test_2

test_parallel_and_destroy_reuse_files=test_parallel_and_destroy_reuse

test_destroy_reuse_files=test_destroy_reuse

# Run the test programs
check: $(test_files) 
	bash run_tests.sh $(test_files)

check_lock: $(lock_test_files)
	bash run_tests.sh $(lock_test_files)

check_barrier: $(barrier_test_files)
	bash run_tests.sh $(barrier_test_files)

check_gs_1: $(gs_test_1_files)
	bash run_tests.sh $(gs_test_1_files)

check_gs_2: $(gs_test_2_files)
	bash run_tests.sh $(gs_test_2_files)

check_pdr: $(test_parallel_and_destroy_reuse_files)
	bash run_tests.sh $(test_parallel_and_destroy_reuse_files)

check_dr: $(test_destroy_reuse_files)
	bash run_tests.sh $(test_destroy_reuse_files)

clean:
	rm -f *.o $(test_files) *~
