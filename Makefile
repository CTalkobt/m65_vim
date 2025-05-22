all: clean vim.prg xfer

VPATH = src

# Common source files
CC=$(HOME)/m65/c/oscar64/bin/oscar64 -tm=mega65 -psci -dNOFLOAT -strict
COPTS=

ASM_SRCS =
C_SRCS = src/*.c

# Object files
OBJS = $(ASM_SRCS:%.s=obj/%.o) $(C_SRCS:%.c=obj/%.o)
OBJS_DEBUG = $(ASM_SRCS:%.s=obj/%-debug.o) $(C_SRCS:%.c=obj/%-debug.o)

vim.prg: src/*.c src/*.h
	$(CC) $(COPTS) -o=$@ src/*.c

clean:
	-rm $(OBJS) $(OBJS:%.o=%.lst) $(OBJS_DEBUG) $(OBJS_DEBUG:%.o=%.lst)
	-rm *.elf *.prg *.lst *.lst

xfer:
	m65 -l /dev/ttyUSB0 -r vim.prg
