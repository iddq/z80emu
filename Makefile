CC = gcc
CFLAGS = -Wall -ansi -pedantic -O2 -fomit-frame-pointer

all: tnc2

tables.h: maketables.c
	$(CC) -Wall $< -o maketables
	./maketables > $@

z80emu.o: z80emu.c z80emu.h z80config.h z80user.h \
	instructions.h macros.h tables.h
	$(CC) $(CFLAGS) -c $<

tnc2.o: tnc2.c tnc2.h z80emu.h z80config.h
	$(CC) -Wall -c $<

OBJECT_FILES = tnc2.o z80emu.o

tnc2: $(OBJECT_FILES)
	$(CC) $(OBJECT_FILES) -o $@

clean:
	rm -f *.o tnc2 maketables
