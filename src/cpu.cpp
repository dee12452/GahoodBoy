#include "cpu.hpp"

Cpu::Cpu()
{
    registers.A = 0x00;
    registers.B = 0x00;
    registers.D = 0x00;
    registers.H = 0x00;
    registers.flags = 0x00;
    registers.C = 0x00;
    registers.E = 0x00;
    registers.L = 0x00;
    registers.stackPointer = GAMEBOY_STACK_POINTER_START;
    registers.programCounter = GAMEBOY_PROGRAM_COUNTER_START;
}

bool Cpu::process(Memory &memory)
{
    const int opCodeCycleDelay = processCurrentOpCode(memory);
    
    if(opCodeCycleDelay >= 0)
    {
        cycleDelay(opCodeCycleDelay);
        return true;
    }
    return false;
}

cycle Cpu::processCurrentOpCode(Memory &memory)
{
    const byte nextOpCode = memory.read(registers.programCounter);
    switch(nextOpCode)
    {
        case 0x00: // NOP
            return NOP(registers.programCounter);
        case 0x01: // LD BC,d16
            return LD16(memory, registers.programCounter, registers.B, registers.C);
        case 0x02: // LD (BC),A
            return LD(memory, registers.programCounter, registers.B, registers.C, registers.A, false, false, true);
        case 0x03: // INC BC
            return INC16(registers.programCounter, registers.B, registers.C);
        case 0x04: // INC B
            return INC(registers.programCounter, registers.flags, registers.B);
        case 0x05: // DEC B
            return DEC(registers.programCounter, registers.flags, registers.B);
        case 0x06: // LD B,d8
            return LD(memory, registers.programCounter, registers.B);
        case 0x07: // RLCA
            return RLCA(registers.programCounter, registers.flags, registers.A);
		case 0x0e: // LD C, d8
			return LD(memory, registers.programCounter, registers.C);
        case 0x11: // LD DE,d16
            return LD16(memory, registers.programCounter, registers.D, registers.E);
        case 0x12: // LD (DE),A
            return LD(memory, registers.programCounter, registers.D, registers.E, registers.A, false, false, true);
        case 0x13: // INC DE
            return INC16(registers.programCounter, registers.D, registers.E);
        case 0x14: // INC D
            return INC(registers.programCounter, registers.flags, registers.D);
        case 0x15: // DEC D
            return DEC(registers.programCounter, registers.flags, registers.D);
        case 0x16: // LD D,d8
            return LD(memory, registers.programCounter, registers.D);
        case 0x17: // RLA
            return RLA(registers.programCounter, registers.flags, registers.A);
        case 0x21: // LD HL,d16
            return LD16(memory, registers.programCounter, registers.H, registers.L);
        case 0x22: // LD (HL+),A
            return LD(memory, registers.programCounter, registers.H, registers.L, registers.A, true, false, true);
        case 0x23: // INC DE
            return INC16(registers.programCounter, registers.H, registers.L);
        case 0x24: // INC H
            return INC(registers.programCounter, registers.flags, registers.H);
        case 0x25: // DEC H
            return DEC(registers.programCounter, registers.flags, registers.H);
        case 0x26: // LD H,d8
            return LD(memory, registers.programCounter, registers.H);
        case 0x31: // LD SP,d16
            return LD16(memory, registers.programCounter, registers.stackPointer);
        case 0x32: // LD (HL-),A
            return LD(memory, registers.programCounter, registers.H, registers.L, registers.A, false, true, true);
        case 0x33: // INC SP
            return INC16(registers.programCounter, registers.stackPointer);
        case 0x34: // INC (HL)
            return INC(memory, registers.programCounter, registers.flags, registers.H, registers.L);
        case 0x35: // DEC (HL)
            return DEC(memory, registers.programCounter, registers.flags, registers.H, registers.L);
        case 0x36: // LD (HL),d8
            return LD(memory, registers.programCounter, registers.H, registers.L);
		case 0xAF: // XOR A
			return XOR(registers.programCounter, registers.flags, registers.A, registers.A);
		case 0xC3:  // JP a16
			return JP(memory, registers.programCounter);
        default:
            Gahood::log("CPU encountered unknown op-code %x at %x", nextOpCode & 0xFF, registers.programCounter & 0xFFFF);
            break;
    }
    return -1;
}

// TODO: Implement clock cycle accuracy
void Cpu::cycleDelay(const cycle) const
{
}