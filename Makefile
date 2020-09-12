CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -Isrc -std=c11
LDLIBS = -lssl -lcrypto

BUILD_DIR ?= ./build
SRC_DIR ?= ./src
TEST_DIR ?= ./tests
TARGET = $(BUILD_DIR)/marvel

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

SOURCES_TESTS = $(wildcard $(TEST_DIR)/*_test.c)
TESTS = $(patsubst %.c,%,$(SOURCES_TESTS))

all: $(TARGET) tests

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $^ -o $@ $(CFLAGS)


.PHONY: tests
tests: LDLIBS = $(OBJECTS)
tests: $(TESTS)
	./tests/runtests.sh

$(TEST_DIR)/str_test: $(TEST_DIR)/str_test.c
	$(CC) $^ -o $@ $(CFLAGS) build/str.o

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(OBJECTS_TESTS) $(TESTS) 
	rm -rf ./build
	rm -f ./tests/error_log.txt
	rm -f ./*.o

