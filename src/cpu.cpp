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
		{
			memory.write(Gahood::addressFromBytes(registers.B, registers.C), registers.A);
			registers.programCounter += 0x01;
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
			registers.programCounter += 0x01;
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
			registers.programCounter += 0x01;
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
			registers.programCounter += 0x01;
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
			registers.programCounter += 0x01;
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
			registers.programCounter += 0x01;
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
			registers.programCounter += 0x01;
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
			registers.programCounter += 0x01;
			return 8;
		}
		case 0x3B: // DEC SP
		{
			registers.stackPointer -= 0x01;
			registers.programCounter += 0x01;
			return 8;
		}
		case 0x3C: // INC A
			return INC(registers.programCounter, registers.flags, registers.A);
		case 0x3D: // DEC A
			return DEC(registers.programCounter, registers.flags, registers.A);
		case 0x3E: // LD A,d8
			return LD(memory, registers.programCounter, registers.A);
		case 0x78: // LD A,B
			return LD(registers.programCounter, registers.A, registers.B);
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
			registers.programCounter += 0x01;
			return 8;
		}
		case 0xEA: // LD (a16),A
		{
			const address addrToWrite = Gahood::addressFromBytes(memory.read(registers.programCounter + 0x02), memory.read(registers.programCounter + 0x01));
			memory.write(addrToWrite, registers.A);
			registers.programCounter += 0x03;
			return 16;
		}
		case 0xF0: // LDH A,(a8)
			return LDH(registers.programCounter, registers.A, memory.read(Gahood::addressFromBytes(0xFF, memory.read(registers.programCounter + 0x01))));
		case 0xFA: // LD A,(a16)
		{
			const address addrToRead = Gahood::addressFromBytes(memory.read(registers.programCounter + 0x02), memory.read(registers.programCounter + 0x01));
			registers.A = memory.read(addrToRead);
			registers.programCounter += 0x03;
			return 16;
		}
		case 0xF2: // LD A,(C)
		{
			registers.A = memory.read(Gahood::addressFromBytes(0xFF, registers.C));
			registers.programCounter += 0x01;
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

// TODO: Implement clock cycle accuracy
void Cpu::cycleDelay(const cycle) const
{
}