CFLAGS=-Wno-builtin-declaration-mismatch

emu: src/main.c src/lib.c
	gcc -ggdb $(CFLAGS) src/main.c src/lib.c -o $@

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean
clean:
	@rm -rf $(BUILD)/* emu asm
