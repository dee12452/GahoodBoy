#include "cpu.hpp"

#include <SDL2/SDL_log.h>
#include "utils.hpp"

///////////////////////
// OpCode operations //
///////////////////////

/* Control Instructions */
static inline uint8_t NOP(uint16_t &programCounter);

/* Jumpers :) */
static inline uint8_t JP(uint16_t &programCounter, uint16_t newAddress);

/* Load, store, move */
static inline uint8_t LD(uint16_t &programCounter, uint8_t &reg, const uint8_t value);
static inline uint8_t LD_ADDR(uint16_t &programCounter, uint8_t &reg, const uint8_t value);
static inline uint8_t LD(uint16_t &programCounter, uint8_t &highReg, uint8_t &lowReg, const uint8_t highData, const uint8_t lowData);

/* Arithmetic */
static inline uint8_t INC(uint16_t &programCounter, uint8_t &reg);
static inline uint8_t SUB(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value);

/* Rotation / Shifts */

///////////////////////////
// End OpCode operations //
///////////////////////////

////////////////////////
// CPU implementation //
////////////////////////

Cpu::Cpu(const uint16_t programCounterStart)
{
    registers.A = 0x00;
    registers.B = 0x00;
    registers.D = 0x00;
    registers.H = 0x00;
    registers.flags = 0x00;
    registers.C = 0x00;
    registers.E = 0x00;
    registers.L = 0x00;
    registers.stackPointer = 0x00;
    registers.programCounter = programCounterStart;
}

bool Cpu::processNext(uint8_t *memory, uint16_t memorySize)
{
    if(!memory || memorySize == 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CPU tried processing empty memory. Was game ROM loaded properly?\n");
        return false;
    }

    if(registers.programCounter > memorySize) 
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CPU program counter is outside the bounds of the memory size\n");
        return false;
    }

    const uint16_t nextOpCode = memory[registers.programCounter];
    const uint8_t opCodeCycleDelay = processOpCode(nextOpCode, memory, memorySize);
    
    if(opCodeCycleDelay)
    {
        cycleDelay(opCodeCycleDelay);
        return true;;
    }
    return false;
}

bool hasSpaceForOperation(uint16_t memorySize, uint16_t programCounter, uint8_t bytesNeeded, const char *opCode)
{
    if(programCounter + bytesNeeded < memorySize)
    {
        return true;
    }
    else
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No Space for %s operation in memory", opCode);
        return false;
    };
}

uint8_t Cpu::processOpCode(uint8_t opCode, uint8_t *memory, uint16_t memorySize)
{
    switch(opCode)
    {
        case 0x00:
            return NOP(registers.programCounter);
        case 0x11:
        {
            if(!hasSpaceForOperation(memorySize, registers.programCounter, 3, "LD")) return 0;
            return LD(registers.programCounter, registers.D, registers.E, memory[registers.programCounter + 0x01], memory[registers.programCounter + 0x02]);
        }
        case 0x2c:
            return INC(registers.programCounter, registers.L);
        case 0x4a:
            return LD(registers.programCounter, registers.C, registers.D);
        case 0x4b:
            return LD(registers.programCounter, registers.C, registers.E);
        case 0x53:
            return LD(registers.programCounter, registers.D, registers.E);
        case 0x55:
            return LD(registers.programCounter, registers.D, registers.L);
        case 0x56:
            return LD_ADDR(registers.programCounter, registers.D, memory[create16Bit(registers.H, registers.L)]); // TODO different clock cycle?
        case 0x57:
            return LD(registers.programCounter, registers.D, registers.A);
        case 0x58:
            return LD(registers.programCounter, registers.E, registers.B);
        case 0x59:
            return LD(registers.programCounter, registers.E, registers.C);
        case 0x5a:
            return LD(registers.programCounter, registers.E, registers.D);
        case 0x5b:
            return LD(registers.programCounter, registers.E, registers.E);
        case 0x6c:
            return LD(registers.programCounter, registers.L, registers.H);
        case 0x6e:
            return LD_ADDR(registers.programCounter, registers.L, memory[create16Bit(registers.H, registers.L)]);
        case 0x6f:
            return LD(registers.programCounter, registers.L, registers.A);
        case 0x75:
            return LD_ADDR(registers.programCounter, memory[create16Bit(registers.H, registers.L)], registers.L);
        case 0x90:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.B);
        case 0x91:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.C);
        case 0xc3:
        {
            if(!hasSpaceForOperation(memorySize, registers.programCounter, 3, "JP")) return 0;
            uint16_t newAddress = memory[registers.programCounter + 0x01];
            newAddress = newAddress << 8;
            newAddress = newAddress | memory[registers.programCounter + 0x02];
            return JP(registers.programCounter, newAddress);
        }
        default:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CPU encountered unknown op-code %x", opCode & 0xff);
    }
    return 0;      
}

// TODO: Implement clock cycle accuracy
void Cpu::cycleDelay(int clocks) const
{
}

////////////////////////////
// End CPU implementation //
////////////////////////////

////////////////////////////
// OpCode implementations //
////////////////////////////

static inline void setCarryFlag(uint8_t &flags)
{
    flags |= 0x10; // Set the C flag to true
}

static inline void setHalfCarryFlag(uint8_t &flags)
{
    flags |= 0x20; // Set the C flag to true
}

static inline void setSubtractFlag(uint8_t &flags)
{
    flags |= 0x40; // Set the C flag to true
}

static inline void setZeroFlag(uint8_t &flags)
{
    flags |= 0x80; // Set the Z flag to true
}

static inline uint8_t NOP(uint16_t &programCounter)
{
    programCounter += 0x01;
    return 4;
}

static inline uint8_t INC(uint16_t &programCounter, uint8_t &reg)
{
    reg += 0x01;
    programCounter += 0x01;
    return 4;
}

static inline uint8_t JP(uint16_t &programCounter, uint16_t newAddress)
{
    programCounter = newAddress;
    return 16; // TODO could this be 12 according to the GB CPU man
}

static inline uint8_t LD(uint16_t &programCounter, uint8_t &reg, const uint8_t value)
{
    reg = value;
    programCounter += 0x01;
    return 4;
}

static inline uint8_t LD(uint16_t &programCounter, uint8_t &highReg, uint8_t &lowReg, const uint8_t highData, const uint8_t lowData)
{
    highReg = highData;
    lowReg = lowData;
    programCounter += 0x03;
    return 4;
}

static inline uint8_t LD_ADDR(uint16_t &programCounter, uint8_t &reg, const uint8_t value)
{
    return LD(programCounter, reg, value) * 2;
}

static inline uint8_t SUB(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value)
{
    setSubtractFlag(flags);
    registerA -= value;
    if(value << 4 > registerA << 4)
    {
        setHalfCarryFlag(flags);
    }
    if(value > registerA)
    {
        registerA = 0;
        setCarryFlag(flags);
    }
    else 
    {
        registerA -= value;
    }
    if(registerA == 0)
    {
        setZeroFlag(flags);
    }

    programCounter += 0x01;
    return 4;
}

////////////////////////////////
// End OpCode implementations //
////////////////////////////////