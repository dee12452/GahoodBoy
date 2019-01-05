#ifndef _GAHOOD_BOY_CPU_HPP_
#define _GAHOOD_BOY_CPU_HPP_

#include <stdint.h>

typedef struct Registers 
{
    uint8_t A; // Accumulator
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t H;
    uint8_t L;

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

        uint8_t processOpCode(uint8_t opCode, uint8_t *memory, uint16_t memorySize);
        void cycleDelay(int clocks) const;
};

#endif
