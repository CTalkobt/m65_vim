all: clean vim.prg xfer

VPATH = src

# Common source files
CC=$(PWD)/oscar64/bin/oscar64 -tm=mega65 -psci -dNOFLOAT
COPTS=

ASM_SRCS =
C_SRCS = src/*.c

# Object files
OBJS = $(ASM_SRCS:%.s=obj/%.o) $(C_SRCS:%.c=obj/%.o)
OBJS_DEBUG = $(ASM_SRCS:%.s=obj/%-debug.o) $(C_SRCS:%.c=obj/%-debug.o)

oscar64/bin/oscar64:
	make -C oscar64/make

vim.prg: oscar64/bin/oscar64 src/*.c src/*.h
	$(CC) $(COPTS) -o=$@ src/*.c

clean:
	-rm $(OBJS) $(OBJS:%.o=%.lst) $(OBJS_DEBUG) $(OBJS_DEBUG:%.o=%.lst)
	-rm *.elf *.prg *.lst *.lst

xfer:
	m65 -l /dev/ttyUSB0 -r vim.prg
