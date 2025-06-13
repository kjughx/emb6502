CFLAGS=-Wno-builtin-declaration-mismatch

SRCS = \
	lib.c \
	main.c

emu: $(SRCS)
	gcc -ggdb $(CFLAGS) $(SRCS) -o $@
