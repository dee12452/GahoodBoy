#ifndef _GAHOOD_BOY_CPU_HPP_
#define _GAHOOD_BOY_CPU_HPP_

#include "opcode.hpp"

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

        bool process(Memory &memory);
    
    private:
        Registers registers;

        cycle processCurrentOpCode(Memory &memory);
        void cycleDelay(const cycle clockCycles) const;
};

#endif
