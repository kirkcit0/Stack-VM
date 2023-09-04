#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "machine.h"

// the program counter for the program
// points to next instruction in the code list
static int pc;

// if true the execution will stop
static bool halt;

// indicates if the program should be tracing the output
static bool tracing;

static bool stop_reading;

// stop_reading will not need to be declared but it does need
// to be given a value in whichever file it is used
// extern bool stop_reading

// provides space for the instructions to be stored
static instruction instr_list[MAX_CODE_LENGTH];

// Function declarations.
void execute_trace(instruction instr);
void print_curr_stat();
void print_registers();
void stackify();
void print_instruction(int addr, instruction instr);
int read_instruction_file(FILE* f);
instruction read_single_instruction(FILE* f);
void machine(const char* filename);
int get_bp();
int get_sp();
int ret_at_x(int addr);
bool illegal_opcode(int opc);
int stack_size();
void push_stack(int val);
int pop_stack();
void replace_top(int val);
void negate();
void allocate_space(int m);
void sto(int o);
void cal(int pc);
void rtn();
int stack_top();
void initialize_stack();
char* mnemonic(int opc);

char* op_codes[NUM_OPCODES] = {"LIT","RTN","CAL","POP","PSI",
"PRM","STO","INC","JMP","JPC","CHO","CHI","HLT","NDB","NEG","ADD",
"SUB","MUL","DIV","MOD","EQL","NEQ","LSS","LEQ","GTR","GEQ","PSP"};


char* mnemonic(int opc)
{
	opc--;
	//opc minus one to get back to the correct place in the array
    if (illegal_opcode(opc))
    {
        fflush(stdout);
        fprintf(stderr, "Illegal Opcode: %d", opc);
        exit(EXIT_FAILURE);
    }
    else
        return op_codes[opc];
}

// Initialize the VM.
void initialize()
{    
    initialize_stack();
    
    pc = 0; 
    halt = false; 
    tracing =  true; 
    stop_reading = false;
}

// Execute the instructions based on the op code.
void execute(instruction instr)
{
    // Increment pc.
    pc++;

    // Initialize int variables for the arithmetic and comparison instructions.
    int a, b = 0;

    // pop_stack function returns an int value, a lot of the calls in the instructions
    // will return an error because we aren't storing the result of 'pop_stack' in an int variable
    switch (instr.op)
    {
        case 1: // LIT
            push_stack(instr.m);
            break;

        case 2: // RTN
            pc = stack_top();  //same as stack[sp-1];
            rtn();
            break;

        case 3: // CAL
            cal(pc);
            pc = instr.m;
            break;

        case 4: // POP
            pop_stack();
            break;

        case 5: // PSI
            //push_stack(stack[stack[sp-1]]);
            //pop_stack();
			push_stack(ret_at_x(get_sp()-1));
            //replace_top(ret_at_x(stack_top()));
            break;
	
        case 6: // PRM
            //push_stack(stack[get_bp()-instr.m]);
            push_stack(ret_at_x(get_bp()-instr.m));
            break;

        case 7: // STO
        	sto(instr.m);
            /*stack[stack[sp-1]+instr.m] = stack[sp-2];
            pop_stack();
            pop_stack();
            */
            break;

        case 8: // INC 
            allocate_space(instr.m);
            break;

        case 9: // JMP 
            pc = stack_top();
            pop_stack();
            break;

        case 10: // JPC
            if (stack_top() != 0)
            {
                pc = instr.m;
            }
            pop_stack();
            break;

        case 11: // CHO
            putc(stack_top(), stdout); //possible error with putting an int to putc? idk
            pop_stack();
            break;

        case 12: // CHI
            push_stack(getc(stdin));
            break;

        case 13: // HLT
            halt = true;
            break;

        case 14: // NDB
            tracing = false;
            break;
        
        case 15: // NEG
            negate();
            break;

        case 16: // ADD
            a = pop_stack();
            b = pop_stack();
            push_stack(a + b);
            break;

        case 17: // SUB
            a = pop_stack();
            b = pop_stack();
            push_stack(a - b);
            break;

        case 18: // MUL
            
            a = pop_stack();
            b = pop_stack();
            pop_stack();
            push_stack(a * b);
            
            break;

        case 19: // DIV
            a = pop_stack();
            b = pop_stack();

            // If dividing by 0, halt and display error.
            if (b == 0)
            {
                halt = true;
                fflush(stdout);
                fprintf(stderr, "Divisor is zero in DIV instruction!"); 
            }

            else
            {
                push_stack(a / b);
            }

            break;

        case 20: // MOD
            a = pop_stack();
            b = pop_stack();

            // If modulo by 0, halt and display error.
            if (b == 0)
            {
                halt = true;
                fflush(stdout);
                fprintf(stderr, "Modulus is zero in MOD instruction!"); 
            }

            else
            {
                push_stack(a % b);
            }

            break;

        case 21: // EQL
            a = pop_stack();
            b = pop_stack();
            push_stack(a == b);
            break;

        case 22: // NEQ
            a = pop_stack();
            b = pop_stack();
            push_stack(a != b);
            break;

        case 23: // LSS
            a = pop_stack();
            b = pop_stack();
            push_stack(a < b);
            break;

        case 24: // LEQ
            a = pop_stack();
            b = pop_stack();
            push_stack(a <= b);
            break;

        case 25: // GTR
            a = pop_stack();
            b = pop_stack();
            push_stack(a > b);
            break;

        case 26: // GEQ
            a = pop_stack();
            b = pop_stack();
            push_stack(a >= b);
            break;

        case 27: // PSP
            push_stack(get_sp());
            break;
    }
}

// will begin running the program. this function both executes
// and prints at the same time in order to keep the stack and 
// registers accurate
void run_prog()
{
    while(!halt)
    {
        execute_trace(instr_list[pc]);
    }
}

// executes the instructions and traces whenever needed
void execute_trace(instruction instr)
{
    if (tracing)
    {   
        fprintf(stdout, "==> addr: ");
        print_instruction(pc, instr);
    }

    // Execute the instruction.
    execute(instr); 

    if (tracing)
    {
        print_curr_stat();
    }
}

void print_curr_stat()
{
    print_registers();
    stackify();
}

// prints the current status of pc, bp, sp, and the stack
void print_registers()
{
    // Print the current status of PC, BP, SP.
    fprintf(stdout, "PC: %d BP: %d SP: %d\n", pc, get_bp(), get_sp());
    
}

void stackify()
{
    //print current status of the stack
    fprintf(stdout, "stack: ");
    for(int i = 0; i <= stack_size(); i++)
    {
        fprintf(stdout, "S[%d]: %d ", i, ret_at_x(i));
    }
    fprintf(stdout, "\n");
}

// opens the instruction file and returns a file pointer
FILE* open_instruction_file(const char* file_name)
{
    
    FILE* f = fopen(file_name, "r");
    
    if (f == NULL)
    {
    	fflush(stdout);
    	fprintf(stderr, "Cannot open file: '%s''", file_name);
        exit(EXIT_FAILURE);
    }

    return f; 
}

// will read all the instructions, store them in the list array,
// and return the size of the program
int read_instruction_file(FILE* f)
{
    int count = 0; 
    while (count < MAX_CODE_LENGTH)
    {
        instruction curr = read_single_instruction(f);
        
        if (stop_reading)
            break;
        
        // Store the instruction in 'instr_list'.
        instr_list[count] = curr;
        count++; 
    } 

    // error check if count is greater than the MAX_CODE_LENGTH
    if (count >= MAX_CODE_LENGTH)
    {
    	fflush(stdout);
    	fprintf(stderr, "Error: too many instructions!");
    	exit(EXIT_FAILURE);
        // code has to exit since it has too many instructions
        // also needs to print to stderr
    }

    return count;
}

// reads only a single instruction from the file and returns an instruction variable
instruction read_single_instruction(FILE* f)
{
    instruction inst; 
    int opcode, m;

    // Read in the individual instruction.
    int num_read = fscanf(f, "%d %d\n", &opcode, &m);
    inst.op = opcode; 
    inst.m = m;

    // if there is an illegal opcode or less than two ints where read, we stop reading
    if (illegal_opcode(inst.op) || num_read < 2)
    { 
        stop_reading = true; 
    }

    return inst;
}

// closes the instruction file and catches if it did not close
void close_instruction_file(FILE* f)
{
    int check = fclose(f);
    if (check == EOF)
    {
        fflush(stdout);
        fprintf(stderr, "Cannot close stream for program file");
        exit(EXIT_FAILURE);
        // file did not close and needs to exit with failure
    }
}

// prints the heading and all address, instruction mnemonics, and m values
void print_program(int size) // ---> size variable? = number of instructions
{
    fprintf(stdout, "%-5s %-5s %-5s\n", "Addr", "OP", "M");
    
    for(int i = 0; i < size; i++)
    {
        print_instruction(i, instr_list[i]); 
    }
}

void print_instruction(int addr, instruction instr)
{
    fprintf(stdout, "%-5d %-5s %-5d\n", addr, mnemonic(instr.op), instr.m);
}

void machine(const char* filename)
{
    // Initialize the VM. 
    initialize();

    // Open and read instruction file.
    FILE * instruction_file = open_instruction_file(filename);
    int num_instructions = read_instruction_file(instruction_file);

    // prints the program
    print_program(num_instructions);
    fprintf(stdout, "Tracing ...\n"); // to be consistent with output
    print_curr_stat();

    // Close instruction file.
    close_instruction_file(instruction_file);
    run_prog();
}

void invariant_check(){
    if(get_bp() <= 0 || get_bp() >= get_sp() || get_sp() > MAX_STACK_HEIGHT){
        fflush(stdout);
        fprintf(stderr, "Invariant Violated");
        exit(EXIT_FAILURE);
    }
    if(pc <= 0 || pc > MAX_CODE_LENGTH){
        fflush(stdout);
        fprintf(stderr, "Invariant Violated");
        exit(EXIT_FAILURE);
    }
}
