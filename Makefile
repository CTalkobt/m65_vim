all: clean vim.prg xfer

VPATH = src

# Common source files
CC=oscar64/bin/oscar64
COPTS=-tm=mega65 -psci -dNOFLOAT
COPTS64=-tm=c64 -psci -dNOFLOAT

ASM_SRCS =
C_SRCS = src/*.c

# Object files
OBJS = $(ASM_SRCS:%.s=obj/%.o) $(C_SRCS:%.c=obj/%.o)
OBJS_DEBUG = $(ASM_SRCS:%.s=obj/%-debug.o) $(C_SRCS:%.c=obj/%-debug.o)

oscar64/bin/oscar64:
	make -C oscar64/make compiler

vim64.prg: oscar64/bin/oscar64 src/*.c src/*.h
	$(CC) $(COPTS64) -o=$@ src/*.c

vim.prg: oscar64/bin/oscar64 src/*.c src/*.h
	$(CC) $(COPTS) -o=$@ src/*.c

clean:
	-rm $(OBJS) $(OBJS:%.o=%.lst) $(OBJS_DEBUG) $(OBJS_DEBUG:%.o=%.lst) 2>/dev/null
	-rm *.elf *.prg *.lst *.lst 2>/dev/null

xfer:
	m65 -l /dev/ttyUSB0 -r vim.prg
