CFLAGS=-Wno-builtin-declaration-mismatch
TRACE ?= 1

emu: src/main.c src/lib.c
	gcc -ggdb -DTRACE=$(TRACE) $(CFLAGS) src/main.c src/lib.c -o $@

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean
clean:
	@rm -rf $(BUILD)/* emu asm
