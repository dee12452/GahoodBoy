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
	IME = false;
}

cycle Cpu::process(Memory &memory)
{
    const byte nextOpCode = memory.read(registers.programCounter);
	registers.programCounter += 0x01;
    switch(nextOpCode)
    {
        case 0x00: // NOP
            return NOP(registers.programCounter);
        case 0x01: // LD BC,d16
            return LD16(memory, registers.programCounter, registers.B, registers.C);
        case 0x02: // LD (BC),A
		{
			memory.write(Gahood::addressFromBytes(registers.B, registers.C), registers.A);
			return 8;
		}
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
        case 0x08: // LD (a16),SP
            return LD16(memory, registers.programCounter, registers.stackPointer, false);
        case 0x09: // ADD HL,BC
            return ADD16(registers.programCounter, registers.flags, registers.H, registers.L, registers.B, registers.C);
		case 0x0A: // LD A,(BC)
		{
			registers.A = memory.read(Gahood::addressFromBytes(registers.B, registers.C));
			return 8;
		}
		case 0x0B: // DEC BC
			return DEC16(registers.programCounter, registers.B, registers.C);
		case 0x0C: // INC C
			return INC(registers.programCounter, registers.flags, registers.C);
		case 0x0D: // DEC C
			return DEC(registers.programCounter, registers.flags, registers.C);
		case 0x0E: // LD C, d8
			return LD(memory, registers.programCounter, registers.C);
        case 0x11: // LD DE,d16
            return LD16(memory, registers.programCounter, registers.D, registers.E);
        case 0x12: // LD (DE),A
		{
			memory.write(Gahood::addressFromBytes(registers.D, registers.E), registers.A);
			return 8;
		}
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
        case 0x19: // ADD HL,DE
            return ADD16(registers.programCounter, registers.flags, registers.H, registers.L, registers.D, registers.E);
		case 0x1A: // LD A,(DE)
		{
			registers.A = memory.read(Gahood::addressFromBytes(registers.D, registers.E));
			return 8;
		}
		case 0x1B: // DEC DE
			return DEC16(registers.programCounter, registers.D, registers.E);
		case 0x1C: // INC E
			return INC(registers.programCounter, registers.flags, registers.E);
		case 0x1D: // DEC E
			return DEC(registers.programCounter, registers.flags, registers.E);
		case 0x1E: // LD E,d8
			return LD(memory, registers.programCounter, registers.E);
		case 0x20: // JR NZ,r8
			return JR(memory, registers.programCounter, !getZeroFlag(registers.flags));
        case 0x21: // LD HL,d16
            return LD16(memory, registers.programCounter, registers.H, registers.L);
        case 0x22: // LD (HL+),A
		{
			const address addrToWrite = Gahood::addressFromBytes(registers.H, registers.L) + 0x01;
			memory.write(addrToWrite, registers.A);
			registers.H = static_cast<byte> ((addrToWrite & 0xFF00) >> 8);
			registers.L = static_cast<byte> (addrToWrite & 0x00FF);
			return 8;
		}
        case 0x23: // INC DE
            return INC16(registers.programCounter, registers.H, registers.L);
        case 0x24: // INC H
            return INC(registers.programCounter, registers.flags, registers.H);
        case 0x25: // DEC H
            return DEC(registers.programCounter, registers.flags, registers.H);
        case 0x26: // LD H,d8
            return LD(memory, registers.programCounter, registers.H);
        case 0x29: // ADD HL,HL
            return ADD16(registers.programCounter, registers.flags, registers.H, registers.L, registers.H, registers.L);
		case 0x2A: // LD A,(HL+)
		{
			const address addrToRead = Gahood::addressFromBytes(registers.H, registers.L) + 0x01;
			registers.A = memory.read(addrToRead);
			registers.H = static_cast<byte> ((addrToRead & 0xFF00) >> 8);
			registers.L = static_cast<byte> (addrToRead & 0x00FF);
			return 8;
		}
		case 0x2B: // DEC HL
			return DEC16(registers.programCounter, registers.H, registers.L);
		case 0x2C: // INC L
			return INC(registers.programCounter, registers.flags, registers.L);
		case 0x2D: // DEC L
			return DEC(registers.programCounter, registers.flags, registers.L);
		case 0x2E: // LD L,d8
			return LD(memory, registers.programCounter, registers.L);
        case 0x31: // LD SP,d16
            return LD16(memory, registers.programCounter, registers.stackPointer, true);
        case 0x32: // LD (HL-),A
		{
			const address addrToWrite = Gahood::addressFromBytes(registers.H, registers.L) - 0x01;
			memory.write(addrToWrite, registers.A);
			registers.H = static_cast<byte> ((addrToWrite & 0xFF00) >> 8);
			registers.L = static_cast<byte> (addrToWrite & 0x00FF);
			return 8;
		}
        case 0x33: // INC SP
            return INC16(registers.programCounter, registers.stackPointer);
        case 0x34: // INC (HL)
            return INC(memory, registers.programCounter, registers.flags, registers.H, registers.L);
        case 0x35: // DEC (HL)
            return DEC(memory, registers.programCounter, registers.flags, registers.H, registers.L);
        case 0x36: // LD (HL),d8
            return LD(memory, registers.programCounter, registers.H, registers.L);
        case 0x39: // ADD HL,SP
            return ADD16(registers.programCounter, registers.flags, registers.H, registers.L, static_cast<byte> (registers.stackPointer >> 8), static_cast<byte> (registers.stackPointer & 0x00FF));
		case 0x3A: // LD A,(HL-)
		{
			const address addrToRead = Gahood::addressFromBytes(registers.H, registers.L) - 0x01;
			registers.A = memory.read(addrToRead);
			registers.H = static_cast<byte> ((addrToRead & 0xFF00) >> 8);
			registers.L = static_cast<byte> (addrToRead & 0x00FF);
			return 8;
		}
		case 0x3B: // DEC SP
		{
			registers.stackPointer -= 0x01;
			return 8;
		}
		case 0x3C: // INC A
			return INC(registers.programCounter, registers.flags, registers.A);
		case 0x3D: // DEC A
			return DEC(registers.programCounter, registers.flags, registers.A);
		case 0x3E: // LD A,d8
			return LD(memory, registers.programCounter, registers.A);
		case 0x40: // LD B,B
			return LD(registers.programCounter, registers.B, registers.B);
		case 0x41: // LD B,C
			return LD(registers.programCounter, registers.B, registers.C);
		case 0x42: // LD B,D
			return LD(registers.programCounter, registers.B, registers.D);
		case 0x43: // LD B,E
			return LD(registers.programCounter, registers.B, registers.E);
		case 0x44: // LD B,H
			return LD(registers.programCounter, registers.B, registers.H);
		case 0x45: // LD B,L
			return LD(registers.programCounter, registers.B, registers.L);
		case 0x46: // LD B,(HL)
		{
			registers.B = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
			return 8;
		}
		case 0x47: // LD B,A
			return LD(registers.programCounter, registers.C, registers.A);
		case 0x48: // LD C,B
			return LD(registers.programCounter, registers.C, registers.B);
		case 0x49: // LD C,C
			return LD(registers.programCounter, registers.C, registers.C);
		case 0x4A: // LD C,D
			return LD(registers.programCounter, registers.C, registers.D);
		case 0x4B: // LD C,E
			return LD(registers.programCounter, registers.C, registers.E);
		case 0x4C: // LD C,H
			return LD(registers.programCounter, registers.C, registers.H);
		case 0x4D: // LD C,L
			return LD(registers.programCounter, registers.C, registers.L);
		case 0x4E: // LD C,(HL)
		{
			registers.C = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
			return 8;
		}
		case 0x4F: // LD C,A
			return LD(registers.programCounter, registers.C, registers.A);
		case 0x50: // LD D,B
			return LD(registers.programCounter, registers.D, registers.B);
		case 0x51: // LD D,C
			return LD(registers.programCounter, registers.D, registers.C);
		case 0x52: // LD D,D
			return LD(registers.programCounter, registers.D, registers.D);
		case 0x53: // LD D,E
			return LD(registers.programCounter, registers.D, registers.E);
		case 0x54: // LD D,H
			return LD(registers.programCounter, registers.D, registers.H);
		case 0x55: // LD D,L
			return LD(registers.programCounter, registers.D, registers.L);
		case 0x56: // LD D,(HL)
		{
			registers.D = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
			return 8;
		}
		case 0x57: // LD D,A
			return LD(registers.programCounter, registers.D, registers.A);
		case 0x58: // LD E,B
			return LD(registers.programCounter, registers.E, registers.B);
		case 0x59: // LD E,C
			return LD(registers.programCounter, registers.E, registers.C);
		case 0x5A: // LD E,D
			return LD(registers.programCounter, registers.E, registers.D);
		case 0x5B: // LD E,E
			return LD(registers.programCounter, registers.E, registers.E);
		case 0x5C: // LD E,H
			return LD(registers.programCounter, registers.E, registers.H);
		case 0x5D: // LD E,L
			return LD(registers.programCounter, registers.E, registers.L);
		case 0x5E: // LD E,(HL)
		{
			registers.E = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
			return 8;
		}
		case 0x5F: // LD E,A
			return LD(registers.programCounter, registers.E, registers.A);
		case 0x60: // LD H,B
			return LD(registers.programCounter, registers.H, registers.B);
		case 0x61: // LD H,C
			return LD(registers.programCounter, registers.H, registers.C);
		case 0x62: // LD H,D
			return LD(registers.programCounter, registers.H, registers.D);
		case 0x63: // LD H,E
			return LD(registers.programCounter, registers.H, registers.E);
		case 0x64: // LD H,H
			return LD(registers.programCounter, registers.H, registers.H);
		case 0x65: // LD H,L
			return LD(registers.programCounter, registers.H, registers.L);
		case 0x66: // LD H,(HL)
		{
			registers.H = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
			return 8;
		}
		case 0x67: // LD H,A
			return LD(registers.programCounter, registers.H, registers.A);
		case 0x68: // LD L,B
			return LD(registers.programCounter, registers.L, registers.B);
		case 0x69: // LD L,C
			return LD(registers.programCounter, registers.L, registers.C);
		case 0x6A: // LD L,D
			return LD(registers.programCounter, registers.L, registers.D);
		case 0x6B: // LD L,E
			return LD(registers.programCounter, registers.L, registers.E);
		case 0x6C: // LD L,H
			return LD(registers.programCounter, registers.L, registers.H);
		case 0x6D: // LD L,L
			return LD(registers.programCounter, registers.L, registers.L);
		case 0x6E: // LD L,(HL)
		{
			registers.L = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
			return 8;
		}
		case 0x6F: // LD L,A
			return LD(registers.programCounter, registers.L, registers.A);
		case 0x70: // LD (HL),B
			return LD(memory, registers.programCounter, registers.H, registers.L, registers.B);
		case 0x71: // LD (HL),C
			return LD(memory, registers.programCounter, registers.H, registers.L, registers.C);
		case 0x72: // LD (HL),D
			return LD(memory, registers.programCounter, registers.H, registers.L, registers.D);
		case 0x73: // LD (HL),E
			return LD(memory, registers.programCounter, registers.H, registers.L, registers.E);
		case 0x74: // LD (HL),H
			return LD(memory, registers.programCounter, registers.H, registers.L, registers.H);
		case 0x75: // LD (HL),L
			return LD(memory, registers.programCounter, registers.H, registers.L, registers.L);
		case 0x77: // LD (HL),A
			return LD(memory, registers.programCounter, registers.H, registers.L, registers.A);
		case 0x78: // LD A,B
			return LD(registers.programCounter, registers.A, registers.B);
		case 0x79: // LD A,C
			return LD(registers.programCounter, registers.A, registers.C);
		case 0x7A: // LD A,D
			return LD(registers.programCounter, registers.A, registers.D);
		case 0x7B: // LD A,E
			return LD(registers.programCounter, registers.A, registers.E);
		case 0x7C: // LD A,H
			return LD(registers.programCounter, registers.A, registers.H);
		case 0x7D: // LD A,L
			return LD(registers.programCounter, registers.A, registers.L);
		case 0x7E: // LD A,(HL)
		{
			registers.A = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
			return 8;
		}
		case 0x7F: // LD A,A
			return LD(registers.programCounter, registers.A, registers.A);
		case 0x80: // ADD A,B
			return ADD(registers.flags, registers.A, registers.B);
		case 0x81: // ADD A,C
			return ADD(registers.flags, registers.A, registers.C);
		case 0x82: // ADD A,D
			return ADD(registers.flags, registers.A, registers.D);
		case 0x83: // ADD A,E
			return ADD(registers.flags, registers.A, registers.E);
		case 0x84: // ADD A,H
			return ADD(registers.flags, registers.A, registers.H);
		case 0x85: // ADD A,L
			return ADD(registers.flags, registers.A, registers.L);
		case 0x86: // ADD A,(HL)
		{
			registers.programCounter += 0x01;
			return ADD(registers.flags, registers.A, memory.read(registers.programCounter - 0x01)) * 2;
		}
		case 0x87: // ADD A,A
			return ADC(registers.flags, registers.A, registers.A);
		case 0x88: // ADC A,B
			return ADC(registers.flags, registers.A, registers.B);
		case 0x89: // ADC A,C
			return ADC(registers.flags, registers.A, registers.C);
		case 0x8A: // ADC A,D
			return ADC(registers.flags, registers.A, registers.D);
		case 0x8B: // ADC A,E
			return ADC(registers.flags, registers.A, registers.E);
		case 0x8C: // ADC A,H
			return ADC(registers.flags, registers.A, registers.H);
		case 0x8D: // ADC A,L
			return ADC(registers.flags, registers.A, registers.L);
		case 0x8E: // ADC A,(HL)
		{
			registers.programCounter += 0x01;
			return ADC(registers.flags, registers.A, memory.read(registers.programCounter - 0x01)) * 2;
		}
		case 0x8F: // ADC A,A
			return ADC(registers.flags, registers.A, registers.A);
		case 0xAF: // XOR A
			return XOR(registers.programCounter, registers.flags, registers.A, registers.A);
		case 0xB1: // OR C
			return OR(registers.programCounter, registers.flags, registers.A, registers.C);
		case 0xBE: // CP (HL)
			return CP(memory, registers.programCounter, registers.flags, registers.A, registers.H, registers.L);
		case 0xC3:  // JP a16
			return JP(memory, registers.programCounter);
		case 0xC9: // RET
			return RET(memory, registers.programCounter, registers.stackPointer);
		case 0xCD: // CALL a16
			return CALL(memory, registers.programCounter, registers.stackPointer);
		case 0xE0: // LDH (a8),A
			return LDH(memory, registers.programCounter, registers.A);
		case 0xE2: // LD (C),A
		{
			memory.write(Gahood::addressFromBytes(0xFF, registers.C), registers.A);
			return 8;
		}
		case 0xEA: // LD (a16),A
		{
			const address addrToWrite = Gahood::addressFromBytes(memory.read(registers.programCounter + 0x01), memory.read(registers.programCounter));
			memory.write(addrToWrite, registers.A);
			registers.programCounter += 0x02;
			return 16;
		}
		case 0xF0: // LDH A,(a8)
			return LDH(registers.programCounter, registers.A, memory.read(Gahood::addressFromBytes(0xFF, memory.read(registers.programCounter))));
		case 0xFA: // LD A,(a16)
		{
			const address addrToRead = Gahood::addressFromBytes(memory.read(registers.programCounter + 0x01), memory.read(registers.programCounter));
			registers.A = memory.read(addrToRead);
			registers.programCounter += 0x02;
			return 16;
		}
		case 0xF2: // LD A,(C)
		{
			registers.A = memory.read(Gahood::addressFromBytes(0xFF, registers.C));
			return 8;
		}
		case 0xF3: // DI
			return DI(registers.programCounter, IME);
		case 0xFB: // EI
			return EI(registers.programCounter, IME);
		case 0xFE: // CP d8
			return CP(memory, registers.programCounter, registers.flags, registers.A);
        default:
            Gahood::log("CPU encountered unknown op-code %x at %x", nextOpCode & 0xFF, registers.programCounter & 0xFFFF);
            break;
    }
    return -1;
}