#include "cpu.hpp"

#include <SDL2/SDL_log.h>
#include "opcode.hpp"

Cpu::Cpu(const uint16_t programCounterStart, uint16_t stackPointerStart)
{
    registers.A = 0x00;
    registers.B = 0x00;
    registers.D = 0x00;
    registers.H = 0x00;
    registers.flags = 0x00;
    registers.C = 0x00;
    registers.E = 0x00;
    registers.L = 0x00;
    registers.stackPointer = stackPointerStart;
    registers.programCounter = programCounterStart;
}

bool Cpu::process(Memory &memory)
{
    const uint16_t nextOpCode = memory.readByte(registers.programCounter);
    const uint8_t opCodeCycleDelay = processOpCode(nextOpCode, memory);
    
    if(opCodeCycleDelay)
    {
        cycleDelay(opCodeCycleDelay);
        return true;;
    }
    return false;
}

uint8_t Cpu::processOpCode(const uint8_t opCode, Memory &memory)
{
    switch(opCode)
    {
        case 0x00:
            return NOP(registers.programCounter);
        case 0x01:
            return LD(registers.programCounter, registers.B, registers.C, memory.readByte(registers.programCounter + 0x01), memory.readByte(registers.programCounter + 0x02));
        case 0x02:
            return LD(registers.programCounter, memory.readByte(create16Bit(registers.B, registers.C)), registers.A, true);
        case 0x03:
            return INC(registers.programCounter, registers.flags, registers.B, registers.C);
        case 0x11:
            return LD(registers.programCounter, registers.D, registers.E, memory.readByte(registers.programCounter + 0x01), memory.readByte(registers.programCounter + 0x02));
        case 0x12:
            return LD(registers.programCounter, memory.readByte(create16Bit(registers.D, registers.E)), registers.A, true);
        case 0x13:
            return INC(registers.programCounter, registers.flags, registers.D, registers.E);
        case 0x18:
            return JR(registers.programCounter, static_cast<signed char> (memory.readByte(registers.programCounter + 0x01)));
        case 0x1d:
            return DEC(registers.programCounter, registers.flags, registers.E);
        case 0x21:
            return LD(registers.programCounter, registers.H, registers.L, memory.readByte(registers.programCounter + 0x01), memory.readByte(registers.programCounter + 0x02));
        case 0x23:
            return INC(registers.programCounter, registers.flags, registers.H, registers.L);
        case 0x2e:
            return LD(registers.programCounter, registers.L, memory.readByte(registers.programCounter + 0x01), true);
        case 0x2c:
            return INC(registers.programCounter, registers.flags, registers.L);
        case 0x2d:
            return DEC(registers.programCounter, registers.flags, registers.L);
        case 0x2f:
            return CPL(registers.programCounter, registers.flags, registers.A);
        case 0x31:
            return LD(registers.programCounter, registers.stackPointer, create16Bit(memory.readByte(registers.programCounter + 0x01), memory.readByte(registers.programCounter + 0x02)));
        case 0x33:
            return INC(registers.programCounter, registers.stackPointer);
        case 0x40:
            return LD(registers.programCounter, registers.B, registers.B, false);
        case 0x41:
            return LD(registers.programCounter, registers.B, registers.C, false);
        case 0x42:
            return LD(registers.programCounter, registers.B, registers.D, false);
        case 0x43:
            return LD(registers.programCounter, registers.B, registers.E, false);
        case 0x44:
            return LD(registers.programCounter, registers.B, registers.H, false);
        case 0x45:
            return LD(registers.programCounter, registers.B, registers.L, false);
        case 0x46:
            return LD(registers.programCounter, registers.B, memory.readByte(create16Bit(registers.H, registers.L)), true);
        case 0x47:
            return LD(registers.programCounter, registers.B, registers.A, false);
        case 0x48:
            return LD(registers.programCounter, registers.C, registers.B, false);
        case 0x49:
            return LD(registers.programCounter, registers.C, registers.C, false);
        case 0x4a:
            return LD(registers.programCounter, registers.C, registers.D, false);
        case 0x4b:
            return LD(registers.programCounter, registers.C, registers.E, false);
        case 0x4c:
            return LD(registers.programCounter, registers.C, registers.H, false);
        case 0x4d:
            return LD(registers.programCounter, registers.C, registers.L, false);
        case 0x4e:
            return LD(registers.programCounter, registers.C, memory.readByte(create16Bit(registers.H, registers.L)), true);
        case 0x4f:
            return LD(registers.programCounter, registers.C, registers.A, false);
        case 0x50:
            return LD(registers.programCounter, registers.D, registers.B, false);
        case 0x51:
            return LD(registers.programCounter, registers.D, registers.C, false);
        case 0x52:
            return LD(registers.programCounter, registers.D, registers.D, false);
        case 0x53:
            return LD(registers.programCounter, registers.D, registers.E, false);
        case 0x55:
            return LD(registers.programCounter, registers.D, registers.L, false);
        case 0x56:
            return LD(registers.programCounter, registers.D, memory.readByte(create16Bit(registers.H, registers.L)), true); // TODO different clock cycle?
        case 0x57:
            return LD(registers.programCounter, registers.D, registers.A, false);
        case 0x58:
            return LD(registers.programCounter, registers.E, registers.B, false);
        case 0x59:
            return LD(registers.programCounter, registers.E, registers.C, false);
        case 0x5a:
            return LD(registers.programCounter, registers.E, registers.D, false);
        case 0x5b:
            return LD(registers.programCounter, registers.E, registers.E, false);
        case 0x5c:
            return LD(registers.programCounter, registers.E, registers.H, false);
        case 0x5d:
            return LD(registers.programCounter, registers.E, registers.L, false);
        case 0x5e:
            return LD(registers.programCounter, registers.E, memory.readByte(create16Bit(registers.H, registers.L)), true);
        case 0x5f:
            return LD(registers.programCounter, registers.E, registers.A, false);
        case 0x60:
            return LD(registers.programCounter, registers.H, registers.B, false);
        case 0x61:
            return LD(registers.programCounter, registers.H, registers.C, false);
        case 0x62:
            return LD(registers.programCounter, registers.H, registers.D, false);
        case 0x63:
            return LD(registers.programCounter, registers.H, registers.E, false);
        case 0x64:
            return LD(registers.programCounter, registers.H, registers.H, false);
        case 0x65:
            return LD(registers.programCounter, registers.H, registers.L, false);
        case 0x66:
            return LD(registers.programCounter, registers.H, memory.readByte(create16Bit(registers.H, registers.L)), true);
        case 0x67:
            return LD(registers.programCounter, registers.H, registers.A, false);
        case 0x68:
            return LD(registers.programCounter, registers.L, registers.B, false);
        case 0x69:
            return LD(registers.programCounter, registers.L, registers.C, false);
        case 0x6a:
            return LD(registers.programCounter, registers.L, registers.D, false);
        case 0x6b:
            return LD(registers.programCounter, registers.L, registers.E, false);
        case 0x6c:
            return LD(registers.programCounter, registers.L, registers.H, false);
        case 0x6e:
            return LD(registers.programCounter, registers.L, memory.readByte(create16Bit(registers.H, registers.L)), true);
        case 0x6f:
            return LD(registers.programCounter, registers.L, registers.A, false);
        case 0x70:
            return LD(registers.programCounter, memory.readByte(create16Bit(registers.H, registers.L)), registers.B, true);
        case 0x71:
            return LD(registers.programCounter, memory.readByte(create16Bit(registers.H, registers.L)), registers.C, true);
        case 0x72:
            return LD(registers.programCounter, memory.readByte(create16Bit(registers.H, registers.L)), registers.D, true);
        case 0x73:
            return LD(registers.programCounter, memory.readByte(create16Bit(registers.H, registers.L)), registers.E, true);
        case 0x74:
            return LD(registers.programCounter, memory.readByte(create16Bit(registers.H, registers.L)), registers.H, true);
        case 0x75:
            return LD(registers.programCounter, memory.readByte(create16Bit(registers.H, registers.L)), registers.L, true);
        case 0x77:
            return LD(registers.programCounter, memory.readByte(create16Bit(registers.H, registers.L)), registers.A, true);
        case 0x78:
            return LD(registers.programCounter, registers.A, registers.B, false);
        case 0x79:
            return LD(registers.programCounter, registers.A, registers.C, false);
        case 0x7a:
            return LD(registers.programCounter, registers.A, registers.D, false);
        case 0x7b:
            return LD(registers.programCounter, registers.A, registers.E, false);
        case 0x7c:
            return LD(registers.programCounter, registers.A, registers.H, false);
        case 0x7d:
            return LD(registers.programCounter, registers.A, registers.L, false);
        case 0x7e:
            return LD(registers.programCounter, registers.A, memory.readByte(create16Bit(registers.H, registers.L)), true);
        case 0x7f:
            return LD(registers.programCounter, registers.A, registers.A, false);
        case 0x90:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.B, false);
        case 0x91:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.C, false);
        case 0x92:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.D, false);
        case 0x93:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.E, false);
        case 0x94:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.H, false);
        case 0x95:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.L, false);
        case 0x96:
            return SUB(registers.programCounter, registers.flags, registers.A, memory.readByte(create16Bit(registers.H, registers.L)), true);
        case 0x97:
            return SUB(registers.programCounter, registers.flags, registers.A, registers.A, false);
        case 0x98:
            return SBC(registers.programCounter, registers.flags, registers.A, registers.B, false);
        case 0x99:
            return SBC(registers.programCounter, registers.flags, registers.A, registers.C, false);
        case 0x9a:
            return SBC(registers.programCounter, registers.flags, registers.A, registers.D, false);
        case 0x9b:
            return SBC(registers.programCounter, registers.flags, registers.A, registers.E, false);
        case 0x9c:
            return SBC(registers.programCounter, registers.flags, registers.A, registers.H, false);
        case 0x9d:
            return SBC(registers.programCounter, registers.flags, registers.A, registers.L, false);
        case 0x9e:
            return SBC(registers.programCounter, registers.flags, registers.A, memory.readByte(create16Bit(registers.H, registers.L)), true);
        case 0x9f:
            return SBC(registers.programCounter, registers.flags, registers.A, registers.A, false);
        case 0xc3:
            return JP(registers.programCounter, create16Bit(memory.readByte(registers.programCounter + 0x01), memory.readByte(registers.programCounter + 0x02)));
        case 0xcd:
        {
            registers.stackPointer--;
            memory.writeByte(memory.readByte(registers.programCounter + 0x02), registers.stackPointer);
            registers.stackPointer--;
            memory.writeByte(memory.readByte(registers.programCounter + 0x01), registers.stackPointer);
            const uint16_t newAddress = create16Bit(memory.readByte(registers.programCounter + 0x01), memory.readByte(registers.programCounter + 0x02));
            return CALL(registers.programCounter, newAddress);
        }
        default:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CPU encountered unknown op-code %x at %x", opCode & 0xFF, registers.programCounter & 0xFF);
            exit(EXIT_FAILURE);
    }
    return 0;      
}

// TODO: Implement clock cycle accuracy
void Cpu::cycleDelay(const uint8_t) const
{
}