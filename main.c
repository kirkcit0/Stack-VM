
#include <stdio.h>
#include <stdlib.h>
#include "machine.h"

void machine(const char* filename);
void usage(const char *cmdname);

int main(int argc, char *argv[])
{
    extern void machine(const char *filename);
    extern void usage(const char *cmdname);

    const char *cmdname = argv[0];
    --argc;
    // exactly 1 non-option argument
    if (argc != 1 || argv[1][0] == '-') {
	usage(cmdname);
    }
    machine(argv[1]);

    return EXIT_SUCCESS;
}

/* Print a usage message on stderr and exit with exit code 1. */
void usage(const char *cmdname)
{
    fprintf(stderr, "Usage: %s code-filename\n", cmdname);
    exit(EXIT_FAILURE);
}