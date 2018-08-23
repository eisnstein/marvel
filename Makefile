CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -I. -std=c11
LDLIBS = -lssl -lcrypto

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

SOURCES_TESTS = $(wildcard tests/*_test.c)
OBJECTS_TESTS = $(patsubst %.c,%.o,$(SOURCES_TESTS))
TESTS = $(patsubst %.o,%,$(OBJECTS_TESTS))

$(info OBJECTS_TESTS is $(OBJECTS_TESTS))

all: main

main: $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS) $(LDLIBS)

$(OBJECTS): $(SOURCES)
	$(CC) -c $^ $(CFLAGS) $(LDLIBS)


.PHONY: tests
tests: LDLIBS =
tests: $(TESTS)
	./tests/run_tests.sh

$(TESTS): $(OBJECTS_TESTS)
	$(CC) $^ str.o $(CFLAGS)

$(OBJECTS_TESTS): $(SOURCES_TESTS)
	$(CC) -c $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(OBJECTS_TESTS) $(TESTS) 
	rm -f ./main
	rm -f ./tests/error_log.txt

