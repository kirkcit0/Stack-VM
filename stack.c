#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "machine.h"

void machine(const char* filename);
bool stack_is_empty();
bool stack_is_full();


static int stack[MAX_STACK_HEIGHT];

// points to next free element in the stack
static int sp; 

// points to the bottom of the stack
static int bp; 

void replace_top(int val)
{
    stack[sp-1]=val;
}

void initialize_stack()
{
    sp = 0;
    bp = 0;

    for (int i = 0; i < MAX_STACK_HEIGHT; i++)
    {
        stack[i] = 0;
    }
}

// returns the number of elements in the stack
int stack_size()
{
    return sp - 1;
}

// checks if stack is empty
bool stack_is_empty()
{
    return sp == 0;
}

// checks if stack is full
// I think the max value for 'sp' would be (MAX_STACK_HEIGHT - 1) since
// MAX_STACK_HEIGHT is the max number of elements the stack can hold
// and sp is index-based (0-based)
bool stack_is_full()
{
    return sp == MAX_STACK_HEIGHT;
}

// pushes something onto the stack
void push_stack(int val)
{
    if (stack_is_full())
    {
        // halt with error message
        //halt = true;
        fflush(stdout);
        fprintf(stderr, "Stack is full. No more space.");
    }

    stack[sp++] = val;
}

/*
// Push function for the arithmetic/comparison instructions. 
// Decrements sp instead of increment.
void decr_push(int val)
{
    stack[sp--] = val;
}
i dont think this is needed. the pops already decrease the sp and this would lose the 
top element that we need.
*/

// will pop the top element of the stack and return it
int pop_stack()
{
    // if (stack_is_empty)
    // {
    
    // }

    return stack[--sp];
}

// Another pop function - sets the top element to 0 and decrements sp. Doesn't return an int value.
// Not sure if we will end up needing
// void pop()
// {
//     // if (stack_is_empty)

//     stack[--sp] = 0;
// }

// Negate function for instruction 15. sp remains the same.
void negate()
{
    stack[sp-1] = -stack[sp-1];
}

// will return the first element without popping
int stack_top()
{
    return stack[sp-1];
}

// allocates m more space onto the stack
// If we need to create more space, we would need to copy the contents
// of the original stack to the new larger stack. ???
void allocate_space(int m)
{
    int new_sp = sp + m;
    
    if (new_sp >= MAX_STACK_HEIGHT)
    {
        //halt = true;
        fflush(stdout);
        fprintf(stderr, "Stack is full. No more space.");
    }
    
    sp = new_sp;
}

// return the value of sp
int get_sp()
{
    return sp; 
}

// return the value of bp
int get_bp()
{
    return bp;
}
//returns the value stored at stack[addr]
int ret_at_x(int addr){
	
    if(addr > MAX_STACK_HEIGHT){
    	return -1;
	}
	
    return stack[addr];

}
//is passed the m value of the instruction and does the sto func
void sto(int o){
	stack[stack[sp-1]+o] = stack[sp-2];
	pop_stack();
    pop_stack();
    //maybe better to pop in main function?
}
//returns from a subroutine that was called
void rtn(){
	bp = stack[sp-2];
    sp = sp - 2;
}
//calls a subroutine and appropriately changes values
void cal(int pc){
	stack[sp] = bp; 
    stack[sp+1] = pc;
    bp = sp;
    sp = sp + 2;
}