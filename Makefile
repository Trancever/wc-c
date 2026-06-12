CC ?= cc
CFLAGS ?= -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion

TARGET := mywc
SRC := mywc.c
TEST_RUNNER := tests/run_tests.sh

.PHONY: all test tests clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test tests:
	bash $(TEST_RUNNER)

clean:
	rm -f $(TARGET)
	rm -rf tmp tests/tmp
