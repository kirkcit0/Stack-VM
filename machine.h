#ifndef _MACHINE_H
#define _MACHINE_H

//max length of implemented stack and the code to be read 
#define MAX_STACK_HEIGHT 2048
#define MAX_CODE_LENGTH 512
#define NUM_OPCODES 27

typedef struct {
    int op; /* opcode */
    int m; /* M */
} instruction;



void execute_trace(instruction instr);
void print_curr_status();
void print_instruction(int addr, instruction instr);
int read_instruction_file(FILE* f);
instruction read_single_instruction(FILE* f);
void machine(const char* filename);

#endif