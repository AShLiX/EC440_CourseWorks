override CFLAGS := -Wall -Werror -std=gnu99 -pedantic -O0 -g -pthread $(CFLAGS)
override LDLIBS := -pthread $(LDLIBS)

tls.o: tls.c tls.h

test_tls.o: test_tls.c tls.h

test_tls: test_tls.o tls.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@

test_tls_files = test_tls

# Run the test programs

check_tls: $(test_tls_files)
	bash run_tests.sh $(test_tls_files)

.PHONY: clean

clean:
	rm -f tls.o
