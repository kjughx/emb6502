CFLAGS=-Wno-builtin-declaration-mismatch -std=c23
BUILD = build/

emu: src/main.c src/lib.c src/lib.h
	gcc -ggdb $(CFLAGS) src/main.c src/lib.c -o $@

test:
	@make --silent -C tests/

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean
clean:
	@rm -rf emu ./$(BUILD)/*
