CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -Isrc -std=c11 -DNDEBUG
LDLIBS = -lssl -lcrypto

BUILD_DIR ?= ./build
SRC_DIR ?= ./src
TEST_DIR ?= ./tests
TARGET = $(BUILD_DIR)/marvel

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

SOURCES_TESTS = $(wildcard $(TEST_DIR)/*_test.c)
TESTS = $(patsubst %.c,%,$(SOURCES_TESTS))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $^ -o $@ $(CFLAGS)


.PHONY: tests
tests: $(TESTS)
	echo "created"
	#./tests/runtests.sh

$(TEST_DIR)/str_test: $(TEST_DIR)/str_test.c $(SRC_DIR)/str.c
	$(CC) $< -o $@ $(CFLAGS) build/str.o

$(TEST_DIR)/env_test: $(TEST_DIR)/env_test.c $(SRC_DIR)/env.c
	$(CC) $< -o $@ $(CFLAGS) build/env.o build/str.o

$(TEST_DIR)/marvel_test: $(TEST_DIR)/marvel_test.c
	$(CC) $< -o $@ $(CFLAGS) build/marvel.o build/env.o build/str.o $(LDLIBS)

$(TEST_DIR)/http_test: $(TEST_DIR)/http_test.c $(SRC_DIR)/http.c $(SRC_DIR)/str.c $(SRC_DIR)/uri.c
	$(CC) $< -o $@ $(CFLAGS) build/http.o build/marvel.o build/env.o build/str.o build/uri.o $(LDLIBS)

$(TEST_DIR)/uri_test: $(TEST_DIR)/uri_test.c $(SRC_DIR)/uri.c $(SRC_DIR)/str.c
	$(CC) $< -o $@ $(CFLAGS) build/uri.o build/str.o

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(OBJECTS_TESTS) $(TESTS) 
	rm -rf ./build
	rm -f ./tests/error_log.txt
	rm -f ./*.o

