CFLAGS = -Wno-builtin-declaration-mismatch -std=c23
BUILD = build/
TESTS = $(patsubst src/%.c, $(BUILD)/%.o, $(wildcard src/*.c))

all: test
	@./test

test: $(TESTS) main.c ../../src/lib.c
	@gcc -O0 -ggdb $(CFLAGS) -I../../src/ $^ -o $@

build/%.o: src/%.c ../../src/lib.c src/test.h
	@gcc -O0 -ggdb $(CFLAGS) -I../../src/ -c $< -o $@

clean:
	@rm ./$(BUILD)/*
