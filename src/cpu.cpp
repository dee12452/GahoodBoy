#include "cpu.hpp"

#include "opcode_prefix.hpp"

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
	if(Gahood::isVerboseMode())
	{
		Gahood::log("Processing %x: %x", registers.programCounter, nextOpCode);
	}
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
            return RLCA(registers.flags, registers.A);
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
		case 0x0F: // RRCA
			return RRCA(registers.flags, registers.A);
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
            return RLA(registers.flags, registers.A);
		case 0x18: // JR r8
			return JR(memory, registers.programCounter, true);
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
		case 0x1F: // RRA
			return RRA(registers.flags, registers.A);
		case 0x20: // JR NZ,r8
			return JR(memory, registers.programCounter, !getZeroFlag(registers.flags));
        case 0x21: // LD HL,d16
            return LD16(memory, registers.programCounter, registers.H, registers.L);
        case 0x22: // LD (HL+),A
		{
			address addrToWrite = Gahood::addressFromBytes(registers.H, registers.L);
			memory.write(addrToWrite, registers.A);
			addrToWrite += 0x01;
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
		case 0x27: // DAA
			return DAA(registers.flags, registers.A);
        case 0x29: // ADD HL,HL
            return ADD16(registers.programCounter, registers.flags, registers.H, registers.L, registers.H, registers.L);
		case 0x30: // JR NC,r8
			return JR(memory, registers.programCounter, !getCarryFlag(registers.flags));
		case 0x2A: // LD A,(HL+)
		{
			address addrToRead = Gahood::addressFromBytes(registers.H, registers.L);
			registers.A = memory.read(addrToRead);
			addrToRead += 0x01;
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
		case 0x2F: // CPL
			return CPL(registers.flags, registers.A);
        case 0x31: // LD SP,d16
            return LD16(memory, registers.programCounter, registers.stackPointer, true);
        case 0x32: // LD (HL-),A
		{
			address addrToWrite = Gahood::addressFromBytes(registers.H, registers.L);
			memory.write(addrToWrite, registers.A);
			addrToWrite = Gahood::sub(addrToWrite, 0x01);
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
			address addrToRead = Gahood::addressFromBytes(registers.H, registers.L);
			registers.A = memory.read(addrToRead);
			addrToRead = Gahood::sub(addrToRead, 0x01);
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
			return ADD(registers.flags, registers.A, memory.read(Gahood::addressFromBytes(registers.H, registers.L))) * 2;
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
			return ADC(registers.flags, registers.A, memory.read(Gahood::addressFromBytes(registers.H, registers.L))) * 2;
		case 0x8F: // ADC A,A
			return ADC(registers.flags, registers.A, registers.A);
		case 0x90: // SUB B
			return SUB(registers.flags, registers.A, registers.B);
		case 0x91: // SUB C
			return SUB(registers.flags, registers.A, registers.C);
		case 0x92: // SUB D
			return SUB(registers.flags, registers.A, registers.D);
		case 0x93: // SUB E
			return SUB(registers.flags, registers.A, registers.E);
		case 0x94: // SUB H
			return SUB(registers.flags, registers.A, registers.H);
		case 0x95: // SUB L
			return SUB(registers.flags, registers.A, registers.L);
		case 0x96: // SUB (HL)
			return SUB(registers.flags, registers.A, memory.read(Gahood::addressFromBytes(registers.H, registers.L))) * 2;
		case 0x97: // SUB A
			return SUB(registers.flags, registers.A, registers.A);
		case 0x98: // SBC B
			return SBC(registers.flags, registers.A, registers.B);
		case 0x99: // SBC C
			return SBC(registers.flags, registers.A, registers.C);
		case 0x9A: // SBC D
			return SBC(registers.flags, registers.A, registers.D);
		case 0x9B: // SBC E
			return SBC(registers.flags, registers.A, registers.E);
		case 0x9C: // SBC H
			return SBC(registers.flags, registers.A, registers.H);
		case 0x9D: // SBC L
			return SBC(registers.flags, registers.A, registers.L);
		case 0x9E: // SBC (HL)
			return SBC(registers.flags, registers.A, memory.read(Gahood::addressFromBytes(registers.H, registers.L))) * 2;
		case 0x9F: // SBC A
			return SBC(registers.flags, registers.A, registers.A);
		case 0xA0: // AND B
			return AND(registers.flags, registers.A, registers.B);
		case 0xA1: // AND C
			return AND(registers.flags, registers.A, registers.C);
		case 0xA2: // AND D
			return AND(registers.flags, registers.A, registers.D);
		case 0xA3: // AND E
			return AND(registers.flags, registers.A, registers.E);
		case 0xA4: // AND H
			return AND(registers.flags, registers.A, registers.H);
		case 0xA5: // AND L
			return AND(registers.flags, registers.A, registers.L);
		case 0xA6: // AND (HL)
			return AND(registers.flags, registers.A, memory.read(Gahood::addressFromBytes(registers.H, registers.L))) * 2;
		case 0xA7: // AND A
			return AND(registers.flags, registers.A, registers.A);
		case 0xA8: // XOR B
			return XOR(registers.flags, registers.A, registers.B);
		case 0xA9: // XOR C
			return XOR(registers.flags, registers.A, registers.C);
		case 0xAA: // XOR D
			return XOR(registers.flags, registers.A, registers.D);
		case 0xAB: // XOR E
			return XOR(registers.flags, registers.A, registers.E);
		case 0xAC: // XOR H
			return XOR(registers.flags, registers.A, registers.H);
		case 0xAD: // XOR L
			return XOR(registers.flags, registers.A, registers.L);
		case 0xAE: // XOR (HL)
			return XOR(registers.flags, registers.A, memory.read(Gahood::addressFromBytes(registers.H, registers.L))) * 2;
		case 0xAF: // XOR A
			return XOR(registers.flags, registers.A, registers.A);
		case 0xB0: // OR B
			return OR(registers.flags, registers.A, registers.B);
		case 0xB1: // OR C
			return OR(registers.flags, registers.A, registers.C);
		case 0xB2: // OR D
			return OR(registers.flags, registers.A, registers.D);
		case 0xB3: // OR E
			return OR(registers.flags, registers.A, registers.E);
		case 0xB4: // OR H
			return OR(registers.flags, registers.A, registers.H);
		case 0xB5: // OR L
			return OR(registers.flags, registers.A, registers.L);
		case 0xB6: // OR (HL)
			return OR(registers.flags, registers.A, memory.read(Gahood::addressFromBytes(registers.H, registers.L))) * 2;
		case 0xB7: // OR A
			return OR(registers.flags, registers.A, registers.A);
		case 0xB8: // CP B
			return CP(registers.flags, registers.A, registers.B);
		case 0xB9: // CP C
			return CP(registers.flags, registers.A, registers.C);
		case 0xBA: // CP D
			return CP(registers.flags, registers.A, registers.D);
		case 0xBB: // CP E
			return CP(registers.flags, registers.A, registers.E);
		case 0xBC: // CP H
			return CP(registers.flags, registers.A, registers.H);
		case 0xBD: // CP L
			return CP(registers.flags, registers.A, registers.L);
		case 0xBE: // CP (HL)
			return CP(memory, registers.programCounter, registers.flags, registers.A, registers.H, registers.L);
		case 0xBF: // CP A
			return CP(registers.flags, registers.A, registers.A);
		case 0xC0: // RET NZ
		{
			if (!getZeroFlag(registers.flags))
			{
				return RET(memory, registers.programCounter, registers.stackPointer) + 4;
			}
			return 8;
		}
		case 0xC1: // POP BC
			return POP(memory, registers.stackPointer, registers.B, registers.C);
		case 0xC3:  // JP a16
			return JP(memory, registers.programCounter);
		case 0xC5: // PUSH BC
			return PUSH(memory, registers.stackPointer, registers.B, registers.C);
		case 0xC7: // RST 00
			return RST(memory, registers.programCounter, registers.stackPointer, 0x00);
		case 0xC8: // RET Z
		{
			if (getZeroFlag(registers.flags))
			{
				return RET(memory, registers.programCounter, registers.stackPointer) + 0x04;
			}
			return 8;
		}
		case 0xC9: // RET
			return RET(memory, registers.programCounter, registers.stackPointer);
		case 0xCB: // PREFIX CB
		{
			const cycle clocks = processPrefix(memory);
			return clocks < 0 ? -1 : clocks + 4;
		}
		case 0xCD: // CALL a16
			return CALL(memory, registers.programCounter, registers.stackPointer);
		case 0xCE: // ADC A,d8
		{
			const byte value = memory.read(registers.programCounter);
			registers.programCounter += 0x01;
			return ADC(registers.flags, registers.A, value);
		}
		case 0xCF: // RST 08
			return RST(memory, registers.programCounter, registers.stackPointer, 0x08);
		case 0xD0: // RET NC
		{
			if (!getCarryFlag(registers.flags))
			{
				return RET(memory, registers.programCounter, registers.stackPointer) + 4;
			}
			return 8;
		}
		case 0xD1: // POP DE
			return POP(memory, registers.stackPointer, registers.D, registers.E);
		case 0xD5: // PUSH DE
			return PUSH(memory, registers.stackPointer, registers.D, registers.E);
		case 0xD6: // SUB d8
		{
			const byte value = memory.read(registers.programCounter);
			registers.programCounter += 0x01;
			return SUB(registers.flags, registers.A, value) * 2;
		}
		case 0xD7: // RST 10
			return RST(memory, registers.programCounter, registers.stackPointer, 0x10);
		case 0xDF: // RST 18
			return RST(memory, registers.programCounter, registers.stackPointer, 0x18);
		case 0xE0: // LDH (a8),A
			return LDH(memory, registers.programCounter, registers.A);
		case 0xE1: // POP HL
			return POP(memory, registers.stackPointer, registers.H, registers.L);
		case 0xE2: // LD (C),A
		{
			memory.write(Gahood::addressFromBytes(0xFF, registers.C), registers.A);
			return 8;
		}
		case 0xE5: // PUSH HL
			return PUSH(memory, registers.stackPointer, registers.H, registers.L);
		case 0xE6: // AND d8
			return AND(memory, registers.programCounter, registers.flags, registers.A);
		case 0xE7: // RST 20
			return RST(memory, registers.programCounter, registers.stackPointer, 0x20);
		case 0xE9: // JP (HL)
			return JP(registers.programCounter, registers.H, registers.L);
		case 0xEA: // LD (a16),A
		{
			const address addrToWrite = Gahood::addressFromBytes(memory.read(registers.programCounter + 0x01), memory.read(registers.programCounter));
			memory.write(addrToWrite, registers.A);
			registers.programCounter += 0x02;
			return 16;
		}
		case 0xEF: // RST 28
			return RST(memory, registers.programCounter, registers.stackPointer, 0x28);
		case 0xF0: // LDH A,(a8)
			return LDH(registers.programCounter, registers.A, memory.read(Gahood::addressFromBytes(0xFF, memory.read(registers.programCounter))));
		case 0xF1: // POP AF
			return POP(memory, registers.stackPointer, registers.A, registers.flags);
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
		case 0xF5: // PUSH AF
			return PUSH(memory, registers.stackPointer, registers.A, registers.flags);
		case 0xF7: // RST 30
			return RST(memory, registers.programCounter, registers.stackPointer, 0x30);
		case 0xFB: // EI
			return EI(registers.programCounter, IME);
		case 0xFE: // CP d8
			return CP(memory, registers.programCounter, registers.flags, registers.A);
		case 0xFF: // RST 38
			return RST(memory, registers.programCounter, registers.stackPointer, 0x38);
        default:
            Gahood::log("CPU encountered unknown op-code %x at %x", nextOpCode & 0xFF, registers.programCounter & 0xFFFF);
            break;
    }
    return -1;
}

cycle Cpu::processPrefix(Memory &memory)
{
	const byte nextOpCode = memory.read(registers.programCounter);
	if(Gahood::isVerboseMode())
	{
		Gahood::log("Processing CB prefix %x: %x", registers.programCounter, nextOpCode);
	}
	registers.programCounter += 0x01;
	switch (nextOpCode)
	{
	case 0x00: // RLC B
		return RLC(registers.flags, registers.B);
	case 0x01: // RLC C
		return RLC(registers.flags, registers.C);
	case 0x02: // RLC D
		return RLC(registers.flags, registers.D);
	case 0x03: // RLC E
		return RLC(registers.flags, registers.E);
	case 0x04: // RLC H
		return RLC(registers.flags, registers.H);
	case 0x05: // RLC L
		return RLC(registers.flags, registers.L);
	case 0x06: // RLC (HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RLC(registers.flags, value);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x07: // RLC A
		return RRC(registers.flags, registers.A);
	case 0x08: // RRC B
		return RRC(registers.flags, registers.B);
	case 0x09: // RRC C
		return RRC(registers.flags, registers.C);
	case 0x0A: // RRC D
		return RRC(registers.flags, registers.D);
	case 0x0B: // RRC E
		return RRC(registers.flags, registers.E);
	case 0x0C: // RRC H
		return RRC(registers.flags, registers.H);
	case 0x0D: // RRC L
		return RRC(registers.flags, registers.L);
	case 0x0E: // RRC (HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RRC(registers.flags, value);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x0F: // RRC A
		return RRC(registers.flags, registers.A);
	case 0x10: // RL B
		return RL(registers.flags, registers.B);
	case 0x11: // RL C
		return RL(registers.flags, registers.C);
	case 0x12: // RL D
		return RL(registers.flags, registers.D);
	case 0x13: // RL E
		return RL(registers.flags, registers.E);
	case 0x14: // RL H
		return RL(registers.flags, registers.H);
	case 0x15: // RL L
		return RL(registers.flags, registers.L);
	case 0x16: // RL (HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RL(registers.flags, value);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x17: // RL A
		return RL(registers.flags, registers.A);
	case 0x18: // RR B
		return RR(registers.flags, registers.B);
	case 0x19: // RR C
		return RR(registers.flags, registers.C);
	case 0x1A: // RR D
		return RR(registers.flags, registers.D);
	case 0x1B: // RR E
		return RR(registers.flags, registers.E);
	case 0x1C: // RR H
		return RR(registers.flags, registers.H);
	case 0x1D: // RR L
		return RR(registers.flags, registers.L);
	case 0x1E: // RR (HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RR(registers.flags, value);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x1F: // RR A
		return RR(registers.flags, registers.A);
	case 0x20: // SLA B
		return SLA(registers.flags, registers.B);
	case 0x21: // SLA C
		return SLA(registers.flags, registers.C);
	case 0x22: // SLA D
		return SLA(registers.flags, registers.D);
	case 0x23: // SLA E
		return SLA(registers.flags, registers.E);
	case 0x24: // SLA H
		return SLA(registers.flags, registers.H);
	case 0x25: // SLA L
		return SLA(registers.flags, registers.L);
	case 0x26: // SLA (HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SLA(registers.flags, value);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x27: // SLA A
		return SLA(registers.flags, registers.A);
	case 0x28: // SRA B
		return SRA(registers.flags, registers.B);
	case 0x29: // SRA C
		return SRA(registers.flags, registers.C);
	case 0x2A: // SRA D
		return SRA(registers.flags, registers.D);
	case 0x2B: // SRA E
		return SRA(registers.flags, registers.E);
	case 0x2C: // SRA H
		return SRA(registers.flags, registers.H);
	case 0x2D: // SRA L
		return SRA(registers.flags, registers.L);
	case 0x2E: // SRA (HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SLA(registers.flags, value);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x2F: // SRA A
		return SLA(registers.flags, registers.A);
	case 0x30: // SWAP B
		return SWAP(registers.flags, registers.B);
	case 0x31: // SWAP C
		return SWAP(registers.flags, registers.C);
	case 0x32: // SWAP D
		return SWAP(registers.flags, registers.D);
	case 0x33: // SWAP E
		return SWAP(registers.flags, registers.E);
	case 0x34: // SWAP H
		return SWAP(registers.flags, registers.H);
	case 0x35: // SWAP L
		return SWAP(registers.flags, registers.L);
	case 0x36: // SWAP (HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SWAP(registers.flags, value);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x37: // SWAP A
		return SWAP(registers.flags, registers.A);
	case 0x38: // SRL B
		return SRL(registers.flags, registers.B);
	case 0x39: // SRL C
		return SRL(registers.flags, registers.C);
	case 0x3A: // SRL D
		return SRL(registers.flags, registers.D);
	case 0x3B: // SRL E
		return SRL(registers.flags, registers.E);
	case 0x3C: // SRL H
		return SRL(registers.flags, registers.H);
	case 0x3D: // SRL L
		return SRL(registers.flags, registers.L);
	case 0x3E: // SRL (HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SRL(registers.flags, value);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x3F: // SRL A
		return SRL(registers.flags, registers.A);
	case 0x40: // BIT 0,B
		return BIT(registers.flags, registers.B, 0);
	case 0x41: // BIT 0,C
		return BIT(registers.flags, registers.C, 0);
	case 0x42: // BIT 0,D
		return BIT(registers.flags, registers.D, 0);
	case 0x43: // BIT 0,E
		return BIT(registers.flags, registers.E, 0);
	case 0x44: // BIT 0,H
		return BIT(registers.flags, registers.H, 0);
	case 0x45: // BIT 0,L
		return BIT(registers.flags, registers.L, 0);
	case 0x46: // BIT 0,(HL)
		return BIT(registers.flags, memory.read(Gahood::addressFromBytes(registers.H, registers.L)), 0) * 2;
	case 0x47: // BIT 0,A
		return BIT(registers.flags, registers.A, 0);
	case 0x48: // BIT 1,B
		return BIT(registers.flags, registers.B, 1);
	case 0x49: // BIT 1,C
		return BIT(registers.flags, registers.C, 1);
	case 0x4A: // BIT 1,D
		return BIT(registers.flags, registers.D, 1);
	case 0x4B: // BIT 1,E
		return BIT(registers.flags, registers.E, 1);
	case 0x4C: // BIT 1,H
		return BIT(registers.flags, registers.H, 1);
	case 0x4D: // BIT 1,L
		return BIT(registers.flags, registers.L, 1);
	case 0x4E: // BIT 1,(HL)
		return BIT(registers.flags, memory.read(Gahood::addressFromBytes(registers.H, registers.L)), 1) * 2;
	case 0x4F: // BIT 1,A
		return BIT(registers.flags, registers.A, 1);
	case 0x50: // BIT 2,B
		return BIT(registers.flags, registers.B, 2);
	case 0x51: // BIT 2,C
		return BIT(registers.flags, registers.C, 2);
	case 0x52: // BIT 2,D
		return BIT(registers.flags, registers.D, 2);
	case 0x53: // BIT 2,E
		return BIT(registers.flags, registers.E, 2);
	case 0x54: // BIT 2,H
		return BIT(registers.flags, registers.H, 2);
	case 0x55: // BIT 2,L
		return BIT(registers.flags, registers.L, 2);
	case 0x56: // BIT 2,(HL)
		return BIT(registers.flags, memory.read(Gahood::addressFromBytes(registers.H, registers.L)), 2) * 2;
	case 0x57: // BIT 2,A
		return BIT(registers.flags, registers.A, 2);
	case 0x58: // BIT 3,B
		return BIT(registers.flags, registers.B, 3);
	case 0x59: // BIT 3,C
		return BIT(registers.flags, registers.C, 3);
	case 0x5A: // BIT 3,D
		return BIT(registers.flags, registers.D, 3);
	case 0x5B: // BIT 3,E
		return BIT(registers.flags, registers.E, 3);
	case 0x5C: // BIT 3,H
		return BIT(registers.flags, registers.H, 3);
	case 0x5D: // BIT 3,L
		return BIT(registers.flags, registers.L, 3);
	case 0x5E: // BIT 3,(HL)
		return BIT(registers.flags, memory.read(Gahood::addressFromBytes(registers.H, registers.L)), 3) * 2;
	case 0x5F: // BIT 3,A
		return BIT(registers.flags, registers.A, 3);
	case 0x60: // BIT 4,B
		return BIT(registers.flags, registers.B, 4);
	case 0x61: // BIT 4,C
		return BIT(registers.flags, registers.C, 4);
	case 0x62: // BIT 4,D
		return BIT(registers.flags, registers.D, 4);
	case 0x63: // BIT 4,E
		return BIT(registers.flags, registers.E, 4);
	case 0x64: // BIT 4,H
		return BIT(registers.flags, registers.H, 4);
	case 0x65: // BIT 4,L
		return BIT(registers.flags, registers.L, 4);
	case 0x66: // BIT 4,(HL)
		return BIT(registers.flags, memory.read(Gahood::addressFromBytes(registers.H, registers.L)), 4) * 2;
	case 0x67: // BIT 4,A
		return BIT(registers.flags, registers.A, 4);
	case 0x68: // BIT 5,B
		return BIT(registers.flags, registers.B, 5);
	case 0x69: // BIT 5,C
		return BIT(registers.flags, registers.C, 5);
	case 0x6A: // BIT 5,D
		return BIT(registers.flags, registers.D, 5);
	case 0x6B: // BIT 5,E
		return BIT(registers.flags, registers.E, 5);
	case 0x6C: // BIT 5,H
		return BIT(registers.flags, registers.H, 5);
	case 0x6D: // BIT 5,L
		return BIT(registers.flags, registers.L, 5);
	case 0x6E: // BIT 5,(HL)
		return BIT(registers.flags, memory.read(Gahood::addressFromBytes(registers.H, registers.L)), 5) * 2;
	case 0x6F: // BIT 5,A
		return BIT(registers.flags, registers.A, 5);
	case 0x70: // BIT 6,B
		return BIT(registers.flags, registers.B, 6);
	case 0x71: // BIT 6,C
		return BIT(registers.flags, registers.C, 6);
	case 0x72: // BIT 6,D
		return BIT(registers.flags, registers.D, 6);
	case 0x73: // BIT 6,E
		return BIT(registers.flags, registers.E, 6);
	case 0x74: // BIT 6,H
		return BIT(registers.flags, registers.H, 6);
	case 0x75: // BIT 6,L
		return BIT(registers.flags, registers.L, 6);
	case 0x76: // BIT 6,(HL)
		return BIT(registers.flags, memory.read(Gahood::addressFromBytes(registers.H, registers.L)), 6) * 2;
	case 0x77: // BIT 6,A
		return BIT(registers.flags, registers.A, 6);
	case 0x78: // BIT 7,B
		return BIT(registers.flags, registers.B, 7);
	case 0x79: // BIT 7,C
		return BIT(registers.flags, registers.C, 7);
	case 0x7A: // BIT 7,D
		return BIT(registers.flags, registers.D, 7);
	case 0x7B: // BIT 7,E
		return BIT(registers.flags, registers.E, 7);
	case 0x7C: // BIT 7,H
		return BIT(registers.flags, registers.H, 7);
	case 0x7D: // BIT 7,L
		return BIT(registers.flags, registers.L, 7);
	case 0x7E: // BIT 7,(HL)
		return BIT(registers.flags, memory.read(Gahood::addressFromBytes(registers.H, registers.L)), 7) * 2;
	case 0x7F: // BIT 7,A
		return BIT(registers.flags, registers.A, 7);
	case 0x80: // RES 0,B
		return RES(registers.B, 0);
	case 0x81: // RES 0,C
		return RES(registers.C, 0);
	case 0x82: // RES 0,D
		return RES(registers.D, 0);
	case 0x83: // RES 0,E
		return RES(registers.E, 0);
	case 0x84: // RES 0,H
		return RES(registers.H, 0);
	case 0x85: // RES 0,L
		return RES(registers.L, 0);
	case 0x86: // RES 0,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RES(value, 0);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x87: // RES 0,A
		return RES(registers.A, 0);
	case 0x88: // RES 1,B
		return RES(registers.B, 1);
	case 0x89: // RES 1,C
		return RES(registers.C, 1);
	case 0x8A: // RES 1,D
		return RES(registers.D, 1);
	case 0x8B: // RES 1,E
		return RES(registers.E, 1);
	case 0x8C: // RES 1,H
		return RES(registers.H, 1);
	case 0x8D: // RES 1,L
		return RES(registers.L, 1);
	case 0x8E: // RES 1,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RES(value, 1);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x8F: // RES 1,A
		return RES(registers.A, 1);
	case 0x90: // RES 2,B
		return RES(registers.B, 2);
	case 0x91: // RES 2,C
		return RES(registers.C, 2);
	case 0x92: // RES 2,D
		return RES(registers.D, 2);
	case 0x93: // RES 2,E
		return RES(registers.E, 2);
	case 0x94: // RES 2,H
		return RES(registers.H, 2);
	case 0x95: // RES 2,L
		return RES(registers.L, 2);
	case 0x96: // RES 2,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RES(value, 2);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x97: // RES 2,A
		return RES(registers.A, 2);
	case 0x98: // RES 3,B
		return RES(registers.B, 3);
	case 0x99: // RES 3,C
		return RES(registers.C, 3);
	case 0x9A: // RES 3,D
		return RES(registers.D, 3);
	case 0x9B: // RES 3,E
		return RES(registers.E, 3);
	case 0x9C: // RES 3,H
		return RES(registers.H, 3);
	case 0x9D: // RES 3,L
		return RES(registers.L, 3);
	case 0x9E: // RES 3,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RES(value, 3);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0x9F: // RES 3,A
		return RES(registers.A, 3);
	case 0xA0: // RES 4,B
		return RES(registers.B, 4);
	case 0xA1: // RES 4,C
		return RES(registers.C, 4);
	case 0xA2: // RES 4,D
		return RES(registers.D, 4);
	case 0xA3: // RES 4,E
		return RES(registers.E, 4);
	case 0xA4: // RES 4,H
		return RES(registers.H, 4);
	case 0xA5: // RES 4,L
		return RES(registers.L, 4);
	case 0xA6: // RES 4,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RES(value, 4);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xA7: // RES 4,A
		return RES(registers.A, 4);
	case 0xA8: // RES 5,B
		return RES(registers.B, 5);
	case 0xA9: // RES 5,C
		return RES(registers.C, 5);
	case 0xAA: // RES 5,D
		return RES(registers.D, 5);
	case 0xAB: // RES 5,E
		return RES(registers.E, 5);
	case 0xAC: // RES 5,H
		return RES(registers.H, 5);
	case 0xAD: // RES 5,L
		return RES(registers.L, 5);
	case 0xAE: // RES 5,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RES(value, 5);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xAF: // RES 5,A
		return RES(registers.A, 5);
	case 0xB0: // RES 6,B
		return RES(registers.B, 6);
	case 0xB1: // RES 6,C
		return RES(registers.C, 6);
	case 0xB2: // RES 6,D
		return RES(registers.D, 6);
	case 0xB3: // RES 6,E
		return RES(registers.E, 6);
	case 0xB4: // RES 6,H
		return RES(registers.H, 6);
	case 0xB5: // RES 6,L
		return RES(registers.L, 6);
	case 0xB6: // RES 6,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RES(value, 6);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xB7: // RES 6,A
		return RES(registers.A, 6);
	case 0xB8: // RES 7,B
		return RES(registers.B, 7);
	case 0xB9: // RES 7,C
		return RES(registers.C, 7);
	case 0xBA: // RES 7,D
		return RES(registers.D, 7);
	case 0xBB: // RES 7,E
		return RES(registers.E, 7);
	case 0xBC: // RES 7,H
		return RES(registers.H, 7);
	case 0xBD: // RES 7,L
		return RES(registers.L, 7);
	case 0xBE: // RES 7,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = RES(value, 7);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xBF: // RES 7,A
		return RES(registers.A, 7);
	case 0xC0: // SET 0,B
		return SET(registers.B, 0);
	case 0xC1: // SET 0,C
		return SET(registers.C, 0);
	case 0xC2: // SET 0,D
		return SET(registers.D, 0);
	case 0xC3: // SET 0,E
		return SET(registers.E, 0);
	case 0xC4: // SET 0,H
		return SET(registers.H, 0);
	case 0xC5: // SET 0,L
		return SET(registers.L, 0);
	case 0xC6: // SET 0,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SET(value, 0);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xC7: // SET 0,A
		return SET(registers.A, 0);
	case 0xC8: // SET 1,B
		return SET(registers.B, 1);
	case 0xC9: // SET 1,C
		return SET(registers.C, 1);
	case 0xCA: // SET 1,D
		return SET(registers.D, 1);
	case 0xCB: // SET 1,E
		return SET(registers.E, 1);
	case 0xCC: // SET 1,H
		return SET(registers.H, 1);
	case 0xCD: // SET 1,L
		return SET(registers.L, 1);
	case 0xCE: // SET 1,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SET(value, 1);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xCF: // SET 1,A
		return SET(registers.A, 1);
	case 0xD0: // SET 2,B
		return SET(registers.B, 2);
	case 0xD1: // SET 2,C
		return SET(registers.C, 2);
	case 0xD2: // SET 2,D
		return SET(registers.D, 2);
	case 0xD3: // SET 2,E
		return SET(registers.E, 2);
	case 0xD4: // SET 2,H
		return SET(registers.H, 2);
	case 0xD5: // SET 2,L
		return SET(registers.L, 2);
	case 0xD6: // SET 2,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SET(value, 2);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xD7: // SET 2,A
		return SET(registers.A, 2);
	case 0xD8: // SET 3,B
		return SET(registers.B, 3);
	case 0xD9: // SET 3,C
		return SET(registers.C, 3);
	case 0xDA: // SET 3,D
		return SET(registers.D, 3);
	case 0xDB: // SET 3,E
		return SET(registers.E, 3);
	case 0xDC: // SET 3,H
		return SET(registers.H, 3);
	case 0xDD: // SET 3,L
		return SET(registers.L, 3);
	case 0xDE: // SET 3,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SET(value, 3);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xDF: // SET 3,A
		return SET(registers.A, 3);
	case 0xE0: // SET 4,B
		return SET(registers.B, 4);
	case 0xE1: // SET 4,C
		return SET(registers.C, 4);
	case 0xE2: // SET 4,D
		return SET(registers.D, 4);
	case 0xE3: // SET 4,E
		return SET(registers.E, 4);
	case 0xE4: // SET 4,H
		return SET(registers.H, 4);
	case 0xE5: // SET 4,L
		return SET(registers.L, 4);
	case 0xE6: // SET 4,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SET(value, 4);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xE7: // SET 4,A
		return SET(registers.A, 4);
	case 0xE8: // SET 5,B
		return SET(registers.B, 5);
	case 0xE9: // SET 5,C
		return SET(registers.C, 5);
	case 0xEA: // SET 5,D
		return SET(registers.D, 5);
	case 0xEB: // SET 5,E
		return SET(registers.E, 5);
	case 0xEC: // SET 5,H
		return SET(registers.H, 5);
	case 0xED: // SET 5,L
		return SET(registers.L, 5);
	case 0xEE: // SET 5,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SET(value, 5);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xEF: // SET 5,A
		return SET(registers.A, 5);
	case 0xF0: // SET 6,B
		return SET(registers.B, 6);
	case 0xF1: // SET 6,C
		return SET(registers.C, 6);
	case 0xF2: // SET 6,D
		return SET(registers.D, 6);
	case 0xF3: // SET 6,E
		return SET(registers.E, 6);
	case 0xF4: // SET 6,H
		return SET(registers.H, 6);
	case 0xF5: // SET 6,L
		return SET(registers.L, 6);
	case 0xF6: // SET 6,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SET(value, 6);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xF7: // SET 6,A
		return SET(registers.A, 6);
	case 0xF8: // SET 7,B
		return SET(registers.B, 7);
	case 0xF9: // SET 7,C
		return SET(registers.C, 7);
	case 0xFA: // SET 7,D
		return SET(registers.D, 7);
	case 0xFB: // SET 7,E
		return SET(registers.E, 7);
	case 0xFC: // SET 7,H
		return SET(registers.H, 7);
	case 0xFD: // SET 7,L
		return SET(registers.L, 7);
	case 0xFE: // SET 7,(HL)
	{
		byte value = memory.read(Gahood::addressFromBytes(registers.H, registers.L));
		const cycle clocks = SET(value, 7);
		memory.write(Gahood::addressFromBytes(registers.H, registers.L), value);
		return clocks * 2;
	}
	case 0xFF: // SET 7,A
		return SET(registers.A, 7);
	default:
		Gahood::log("CPU encountered unknown prefix op-code %x at %x", nextOpCode & 0xFF, registers.programCounter & 0xFFFF);
		break;
	}
	return -1;
}