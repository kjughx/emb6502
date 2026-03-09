CFLAGS=-Wno-builtin-declaration-mismatch

BUILD = ./build/
SRCS = $(filter-out src/configuration.s src/macros.s src/setup.s, $(wildcard src/*.s))

OBJS = $(SRCS:src/%.s=build/%.bin)

all: $(OBJS) test

test: main.c ../../src/lib.c FORCE
	@gcc -ggdb $(CFLAGS) -I ../../src/ main.c ../../src/lib.c -o $@
	@./test

build/%.bin: src/%.s
	./cc65/bin/ca65 -l $(BUILD)/$*.lst $< -o $(BUILD)/$*.o
	./cc65/bin/ld65 $(BUILD)/$*.o -o $@ -C script.ld
	@rm -f $(BUILD)/$*.o $(BUILD)/$*.map

FORCE:

clean:
	@rm -rf ./$(BUILD)/*.bin ./test
