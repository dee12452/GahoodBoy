#ifndef _GAHOOD_BOY_CPU_HPP_
#define _GAHOOD_BOY_CPU_HPP_

#include <stdint.h>

typedef struct Registers 
{
    uint16_t A; // Accumulator
    uint16_t B;
    uint16_t D;
    uint16_t H;

    /* Flag register bits:
    7 6 5 4 3 2 1 0
    Z N H C 0 0 0 0

    Z = Zero Flag
    N = Subtract Flag
    H = Half Carry Flag
    C = Carry Flag 
    0 = Not used, always 0
    */
    uint8_t flags; 
    uint8_t C;
    uint8_t E;
    uint8_t L;

    uint16_t stackPointer;
    uint16_t programCounter;
} Registers;

class Cpu
{
    public:
        Cpu(const uint16_t programCounterStart);

        bool processNext(uint8_t *memory, uint16_t memorySize);
    
    private:
        Registers registers;
};

#endif
