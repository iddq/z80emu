/* zextest.c
 * Example program using z80emu to run the zexall and zexdoc tests. This will 
 * check if the Z80 is correctly emulated.
 *
 * Copyright (c) 2012, 2016 Lin Ke-Fong
 * Copyright (c) 2012 Chris Pressey
 *
 * This code is free, do whatever you want with it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "zextest.h"
#include "z80emu.h"

#define Z80_CPU_SPEED           4000000   /* In Hz. */
#define CYCLES_PER_STEP         (Z80_CPU_SPEED / 50)
#define MAXIMUM_STRING_LENGTH   100

static void	emulate (char *filename);

int main (void)
{
	time_t	start, stop;

	start = time(NULL);
        emulate("testfiles/TNC2118A.ROM");
	stop = time(NULL);
	printf("Emulating TNC2118A.ROM took a total of %d second(s).\n",
		(int) (stop - start));

        return EXIT_SUCCESS;
}


/* Emulate "TNC2118A.ROM". */

static void emulate (char *filename)
{
ZEXTEST context;
        FILE   	*file;
        long   	l;
        double 	total;

        printf("Testing \"%s\"...\n", filename);
        if ((file = fopen(filename, "rb")) == NULL) {

                fprintf(stderr, "Can't open file!\n");
                exit(EXIT_FAILURE);

        }
        fseek(file, 0, SEEK_END);
        l = ftell(file);

        fseek(file, 0, SEEK_SET);
        fread(context.memory + 0x0, 1, l, file);

        fclose(file);

        /* Patch the memory of the program. Reset at 0x0000 is trapped by an
         * OUT which will stop emulation. CP/M bdos call 5 is trapped by an IN.
	 * See Z80_INPUT_BYTE() and Z80_OUTPUT_BYTE() definitions in z80user.h.
         */

#if 0
        context.memory[0] = 0xd3;       /* OUT N, A */
        context.memory[1] = 0x00;

        context.memory[5] = 0xdb;       /* IN A, N */
        context.memory[6] = 0x00;
        context.memory[7] = 0xc9;       /* RET */
#endif

	context.is_done = 0;

        /* Emulate. */

        Z80Reset(&context.state);
        context.state.pc = 0x0;
        total = 0.0;
	do

                total += Z80Emulate(&context.state, CYCLES_PER_STEP, &context);

	while (!context.is_done);

        printf("\n%.0f cycle(s) emulated.\n" 
                "For a Z80 running at %.2fMHz, "
                "that would be %d second(s) or %.2f hour(s).\n",
                total,
                Z80_CPU_SPEED / 1000000.0,
                (int) (total / Z80_CPU_SPEED),
                total / ((double) 3600 * Z80_CPU_SPEED));
}

int portvalues[256];

void port_out(ZEXTEST *context, int port, int value) {
        portvalues[port] = value;
        printf("out: %d value: %c %d, PC: 0x%x\n", port, (value>31 && value<128 ? value : '.'), value, context->state.pc);
}

int port_in(ZEXTEST *context, int port) {
        int value;
        value = portvalues[port];
        printf(" in: %d value: %d\n", port, value);
        return value;
}

/* Emulate CP/M bdos call 5 functions 2 (output character on screen) and 9
 * (output $-terminated string to screen).
 */

void SystemCall (ZEXTEST *zextest)
{

        printf("SystemCall\n");

/*
        if (zextest->state.registers.byte[Z80_C] == 2)

                printf("%c", zextest->state.registers.byte[Z80_E]);

        else if (zextest->state.registers.byte[Z80_C] == 9) {

                int     i, c;

                for (i = zextest->state.registers.word[Z80_DE], c = 0; 
                        zextest->memory[i] != '$';
                        i++) {

                        printf("%c", zextest->memory[i & 0xffff]);
                        if (c++ > MAXIMUM_STRING_LENGTH) {

                                fprintf(stderr,
                                        "String to print is too long!\n");
                                exit(EXIT_FAILURE);

                        }

                }

        }
*/
}
