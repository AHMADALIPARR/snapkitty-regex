CC = cc
CFLAGS = -O2 -std=c99 -Wall -Wextra -Wpedantic
PREFIX ?= /usr/local

all: regex

regex: src/regex.c src/main.c src/regex.h
	$(CC) $(CFLAGS) -o $@ src/regex.c src/main.c

test: regex
	python3 tests/test_regex.py

bench: regex
	python3 tests/bench.py

clean:
	rm -f regex

install: regex
	install -m 755 regex $(PREFIX)/bin/regex

.PHONY: all test bench clean install
