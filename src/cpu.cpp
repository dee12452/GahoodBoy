#include "cpu.hpp"

#include <SDL2/SDL_log.h>
#include "utils.hpp"

///////////////////////
// OpCode operations //
///////////////////////

/* Control Instructions */
static uint8_t NOP(uint16_t &programCounter);

/* Jumpers :) */
static uint8_t JP(uint16_t &programCounter, uint16_t newAddress);
static uint8_t JR(uint16_t &programCounter, const signed char addToCounter);

/* Load, store, move */
static uint8_t LD(uint16_t &programCounter, uint8_t &reg, const uint8_t value);
static uint8_t LD_ADDR(uint16_t &programCounter, uint8_t &reg, const uint8_t value);
static uint8_t LD_D(uint16_t &programCounter, uint8_t &reg, const uint8_t value);
static uint8_t LD_D(uint16_t &programCounter, uint8_t &highReg, uint8_t &lowReg, const uint8_t highData, const uint8_t lowData);
static uint8_t LD_D(uint16_t &programCounter, uint16_t &reg, const uint16_t value);

/* Arithmetic */
static uint8_t INC(uint16_t &programCounter, uint8_t &flags, uint8_t &reg);
static uint8_t DEC(uint16_t &programCounter, uint8_t &flags, uint8_t &reg);
static uint8_t SUB(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value);
static uint8_t SUB_ADDR(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value);
static uint8_t SUBC(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value);
static uint8_t SUBC_ADDR(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value);
static uint8_t CPL(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA);

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
        case 0x01:
        {
            if(!hasSpaceForOperation(memorySize, registers.programCounter, 3, "LD_D")) return 0;
            return LD_D(registers.programCounter, registers.B, registers.C, memory[registers.programCounter + 0x01], memory[registers.programCounter + 0x02]);
        }
        case 0x11:
        {
            if(!hasSpaceForOperation(memorySize, registers.programCounter, 3, "LD_D")) return 0;
            return LD_D(registers.programCounter, registers.D, registers.E, memory[registers.programCounter + 0x01], memory[registers.programCounter + 0x02]);
        }
        case 0x18:
            if(!hasSpaceForOperation(memorySize, registers.programCounter, 2, "JR")) return 0;
            return JR(registers.programCounter, static_cast<signed char>(memory[registers.programCounter + 0x01]));
        case 0x1d:
            return DEC(registers.programCounter, registers.flags, registers.E);
        case 0x21:
        {
            if(!hasSpaceForOperation(memorySize, registers.programCounter, 3, "LD_D")) return 0;
            return LD_D(registers.programCounter, registers.H, registers.L, memory[registers.programCounter + 0x01], memory[registers.programCounter + 0x02]);
        }
        case 0x2e:
        {
            if(!hasSpaceForOperation(memorySize, registers.programCounter, 2, "LD_D")) return 0;
            return LD_D(registers.programCounter, registers.L, memory[registers.programCounter + 0x01]);
        }
        case 0x2c:
            return INC(registers.programCounter, registers.flags, registers.L);
        case 0x2d:
            return DEC(registers.programCounter, registers.flags, registers.L);
        case 0x2f:
            return CPL(registers.programCounter, registers.flags, registers.A);
        case 0x31:
        {
            if(!hasSpaceForOperation(memorySize, registers.programCounter, 3, "LD_D")) return 0;
            return LD_D(registers.programCounter, registers.stackPointer, create16Bit(memory[registers.programCounter + 0x01], memory[registers.programCounter + 0x02]));
        }
        case 0x40:
            return LD(registers.programCounter, registers.B, registers.B);
        case 0x41:
            return LD(registers.programCounter, registers.B, registers.C);
        case 0x42:
            return LD(registers.programCounter, registers.B, registers.D);
        case 0x43:
            return LD(registers.programCounter, registers.B, registers.E);
        case 0x44:
            return LD(registers.programCounter, registers.B, registers.H);
        case 0x45:
            return LD(registers.programCounter, registers.B, registers.L);
        case 0x46:
            return LD_ADDR(registers.programCounter, registers.B, memory[create16Bit(registers.H, registers.L)]);
        case 0x47:
            return LD(registers.programCounter, registers.B, registers.A);
        case 0x48:
            return LD(registers.programCounter, registers.C, registers.B);
        case 0x49:
            return LD(registers.programCounter, registers.C, registers.C);
        case 0x4a:
            return LD(registers.programCounter, registers.C, registers.D);
        case 0x4b:
            return LD(registers.programCounter, registers.C, registers.E);
        case 0x4c:
            return LD(registers.programCounter, registers.C, registers.H);
        case 0x4d:
            return LD(registers.programCounter, registers.C, registers.L);
        case 0x4e:
            return LD_ADDR(registers.programCounter, registers.C, memory[create16Bit(registers.H, registers.L)]);
        case 0x4f:
            return LD(registers.programCounter, registers.C, registers.A);
        case 0x50:
            return LD(registers.programCounter, registers.D, registers.B);
        case 0x51:
            return LD(registers.programCounter, registers.D, registers.C);
        case 0x52:
            return LD(registers.programCounter, registers.D, registers.D);
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
        case 0x5c:
            return LD(registers.programCounter, registers.E, registers.H);
        case 0x5d:
            return LD(registers.programCounter, registers.E, registers.L);
        case 0x5e:
            return LD_ADDR(registers.programCounter, registers.E, memory[create16Bit(registers.H, registers.L)]);
        case 0x5f:
            return LD(registers.programCounter, registers.E, registers.A);
        case 0x60:
            return LD(registers.programCounter, registers.H, registers.B);
        case 0x61:
            return LD(registers.programCounter, registers.H, registers.C);
        case 0x62:
            return LD(registers.programCounter, registers.H, registers.D);
        case 0x63:
            return LD(registers.programCounter, registers.H, registers.E);
        case 0x64:
            return LD(registers.programCounter, registers.H, registers.H);
        case 0x65:
            return LD(registers.programCounter, registers.H, registers.L);
        case 0x66:
            return LD_ADDR(registers.programCounter, registers.H, memory[create16Bit(registers.H, registers.L)]);
        case 0x67:
            return LD(registers.programCounter, registers.H, registers.A);
        case 0x68:
            return LD(registers.programCounter, registers.L, registers.B);
        case 0x69:
            return LD(registers.programCounter, registers.L, registers.C);
        case 0x6a:
            return LD(registers.programCounter, registers.L, registers.D);
        case 0x6b:
            return LD(registers.programCounter, registers.L, registers.E);
        case 0x6c:
            return LD(registers.programCounter, registers.L, registers.H);
        case 0x6e:
            return LD_ADDR(registers.programCounter, registers.L, memory[create16Bit(registers.H, registers.L)]);
        case 0x6f:
            return LD(registers.programCounter, registers.L, registers.A);
        case 0x70:
            return LD_ADDR(registers.programCounter, memory[create16Bit(registers.H, registers.L)], registers.B);
        case 0x71:
            return LD_ADDR(registers.programCounter, memory[create16Bit(registers.H, registers.L)], registers.C);
        case 0x72:
            return LD_ADDR(registers.programCounter, memory[create16Bit(registers.H, registers.L)], registers.D);
        case 0x73:
            return LD_ADDR(registers.programCounter, memory[create16Bit(registers.H, registers.L)], registers.E);
        case 0x74:
            return LD_ADDR(registers.programCounter, memory[create16Bit(registers.H, registers.L)], registers.H);
        case 0x75:
            return LD_ADDR(registers.programCounter, memory[create16Bit(registers.H, registers.L)], registers.L);
        case 0x77:
            return LD_ADDR(registers.programCounter, memory[create16Bit(registers.H, registers.L)], registers.A);
        case 0x78:
            return LD(registers.programCounter, registers.A, registers.B);
        case 0x79:
            return LD(registers.programCounter, registers.A, registers.C);
        case 0x7a:
            return LD(registers.programCounter, registers.A, registers.D);
        case 0x7b:
            return LD(registers.programCounter, registers.A, registers.E);
        case 0x7c:
            return LD(registers.programCounter, registers.A, registers.H);
        case 0x7d:
            return LD(registers.programCounter, registers.A, registers.L);
        case 0x7e:
            return LD_ADDR(registers.programCounter, registers.A, memory[create16Bit(registers.H, registers.L)]);
        case 0x7f:
            return LD(registers.programCounter, registers.A, registers.A);
        case 0x90:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.B);
        case 0x91:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.C);
        case 0x92:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.D);
        case 0x93:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.E);
        case 0x94:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.H);
        case 0x95:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.L);
        case 0x96:
            return SUB_ADDR(registers.programCounter, registers.flags, registers.A, memory[create16Bit(registers.H, registers.L)]);
        case 0x97:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.A);
        case 0x98:
            return SUBC(registers.programCounter, registers.flags, registers.A, registers.B);
        case 0x99:
            return SUBC(registers.programCounter, registers.flags, registers.A, registers.C);
        case 0x9a:
            return SUBC(registers.programCounter, registers.flags, registers.A, registers.D);
        case 0x9b:
            return SUBC(registers.programCounter, registers.flags, registers.A, registers.E);
        case 0x9c:
            return SUBC(registers.programCounter, registers.flags, registers.A, registers.H);
        case 0x9d:
            return SUBC(registers.programCounter, registers.flags, registers.A, registers.L);
        case 0x9e:
            return SUBC_ADDR(registers.programCounter, registers.flags, registers.A, memory[create16Bit(registers.H, registers.L)]);
        case 0x9f:
            return SUBC(registers.programCounter, registers.flags, registers.A, registers.A);
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

static void setCarryFlag(uint8_t &flags, bool on)
{
    if(on)
    {
        flags |= 0x10; // Set the C flag to true
    }
    else
    {
        flags &= 0xE0;
    }
}

static void setHalfCarryFlag(uint8_t &flags, bool on)
{
    if(on)
    {
        flags |= 0x20; // Set the C flag to true
    }
    else
    {
        flags &= 0xD0;
    }
}

static void setSubtractFlag(uint8_t &flags, bool on)
{
    if(on)
    {
        flags |= 0x40; // Set the C flag to true
    }
    else
    {
        flags &= 0xB0; // Set the C flag to true
    }
}

static void setZeroFlag(uint8_t &flags, bool on)
{
    if(on)
    {
        flags |= 0x80; // Set the Z flag to true
    }
    else
    {
        flags &= 0x70; // Set the C flag to true
    }
}

static uint8_t NOP(uint16_t &programCounter)
{
    programCounter += 0x01;
    return 4;
}

static uint8_t INC(uint16_t &programCounter, uint8_t &flags, uint8_t &reg)
{
    setSubtractFlag(flags, false);
    setHalfCarryFlag(flags, (reg & 0x07) == 0x07);
    reg += 0x01;
    setZeroFlag(flags, reg == 0x00);
    programCounter += 0x01;
    return 4;
}

static uint8_t DEC(uint16_t &programCounter, uint8_t &flags, uint8_t &reg)
{
    setSubtractFlag(flags, true);
    setHalfCarryFlag(flags, (reg & 0x07) == 0x00);
    reg -= 0x01;
    setZeroFlag(flags, reg == 0x00);
    programCounter += 0x01;
    return 4;
}

static uint8_t JP(uint16_t &programCounter, uint16_t newAddress)
{
    programCounter = newAddress;
    return 16; // TODO could this be 12 according to the GB CPU man
}

static uint8_t JR(uint16_t &programCounter, const signed char addToCounter)
{
    programCounter += addToCounter;
    return 12;
}

static uint8_t LD(uint16_t &programCounter, uint8_t &reg, const uint8_t value)
{
    reg = value;
    programCounter += 0x01;
    return 4;
}

static uint8_t LD_ADDR(uint16_t &programCounter, uint8_t &reg, const uint8_t value)
{
    return LD(programCounter, reg, value) * 2;
}

static uint8_t LD_D(uint16_t &programCounter, uint8_t &highReg, uint8_t &lowReg, const uint8_t highData, const uint8_t lowData)
{
    highReg = highData;
    lowReg = lowData;
    programCounter += 0x03;
    return 12;
}

static uint8_t LD_D(uint16_t &programCounter, uint16_t &reg, const uint16_t value)
{
    reg = value;
    programCounter += 0x03;
    return 12;
}

static uint8_t LD_D(uint16_t &programCounter, uint8_t &reg, const uint8_t value)
{
    programCounter += 0x01;
    return LD(programCounter, reg, value) * 2;
}

static uint8_t SUB(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value)
{
    setSubtractFlag(flags, true);
    registerA -= value;
    setHalfCarryFlag(flags, (value << 4) > (registerA << 4));
    setCarryFlag(flags, (value > registerA));
    registerA -= value;
    setZeroFlag(flags, registerA == 0x00);

    programCounter += 0x01;
    return 4;
}

static uint8_t SUB_ADDR(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value)
{
    return SUB(programCounter, flags, registerA, value) * 2;
}

static uint8_t SUBC(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value)
{
    const bool carryOn = (flags & 0x10) == 0x10;
    uint8_t valueToSubtract = carryOn ? value + 1 : value; // Add 1 to value if carry flag is on
    return SUB(programCounter, flags, registerA, value);
}

static uint8_t SUBC_ADDR(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value)
{
    return SUBC(programCounter, flags, registerA, value) * 2;
}

static uint8_t CPL(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA)
{
    setSubtractFlag(flags, true);
    setHalfCarryFlag(flags, true);
    registerA = ~registerA;
    programCounter += 0x01;
    return 4;
}

////////////////////////////////
// End OpCode implementations //
////////////////////////////////