#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "machine.h"

//const char* mnemonic(int opc);

// is given an opcode and wiil return true if it is illegal
// ---> 0 is also an invalid op code so added '<= 0'
bool illegal_opcode(int opc)
{
    return opc < 0 || opc > NUM_OPCODES;
}

