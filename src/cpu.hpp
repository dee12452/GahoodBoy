#ifndef _GAHOOD_BOY_CPU_HPP_
#define _GAHOOD_BOY_CPU_HPP_

#include "util.hpp"

typedef struct Registers 
{
    byte A; // Accumulator
    byte B;
    byte C;
    byte D;
    byte E;
    byte H;
    byte L;

    /* Flag register bits:
    7 6 5 4 3 2 1 0
    Z N H C 0 0 0 0

    Z = Zero Flag
    N = Subtract Flag
    H = Half Carry Flag
    C = Carry Flag 
    0 = Not used, always 0
    */
    byte flags; 

    address stackPointer;
    address programCounter;
} Registers;

class Memory;

class Cpu
{
public:
    Cpu();

    cycle update(Memory &memory);

private:
    Registers registers;
    bool IME;

    void checkInterrupts(Memory &memory);
    void initiateInterrupt(Memory &memory, const byte IF, const BitNumber interruptBit, const address callAddress);
    cycle processNext(Memory &memory);
    cycle processNextPrefix(Memory &memory);
};

#endif
