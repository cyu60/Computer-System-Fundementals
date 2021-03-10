#ifndef CSIM_H
#define CSIM_H

#include <string>

using std::string;

struct MemAccess 
{
    /* The first field is either l or s depending on whether the processor is “loading” from or “storing” to memory. */
    char operation;
    /* The second field is a 32-bit memory address given in hexadecimal; 
        the 0x at the beginning means “the following is hexadecimal” and is not itself part of the address */
    string address;
    char dummy;
};

#endif