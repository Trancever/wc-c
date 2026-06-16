CC ?= cc
CPPFLAGS ?= -Ilib
CFLAGS ?= -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion

TARGET := mywc
SRC := mywc.c lib/utf_8.c lib/unicode.c
HEADERS := lib/utf_8.h lib/unicode.h
TEST_RUNNER := tests/run_tests.sh

.PHONY: all test tests clean

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $(TARGET) $(SRC)

test tests:
	bash $(TEST_RUNNER)

clean:
	rm -f $(TARGET)
	rm -rf tmp tests/tmp
