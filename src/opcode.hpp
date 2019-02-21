#ifndef _GAHOOD_BOY_OPCODE_HPP_
#define _GAHOOD_BOY_OPCODE_HPP_

#include "memory.hpp"

inline bool getCarryFlag(const byte flags);
inline void setCarryFlag(byte &flags, const bool on);
inline bool getHalfCarryFlag(const byte flags);
inline void setHalfCarryFlag(byte &flags, const bool on);
inline bool getSubtractFlag(const byte flags);
inline void setSubtractFlag(byte &flags, const bool on);
inline void setZeroFlag(byte &flags, const bool on);
inline bool getZeroFlag(const byte flags);

/* Control Instructions */
inline cycle NOP(address &programCounter)
{
    return 4;
}

inline cycle DI(address &programCounter, bool &IME)
{
    IME = false;
	return 4;
}

inline cycle EI(address &programCounter, bool &IME)
{
    IME = true;
	return 4;
}

/* Load, store, move */
inline cycle LD(address &programCounter, byte &reg1, const byte reg2)
{
    reg1 = reg2;
    return 4;
}

inline cycle LD(Memory &memory, address &programCounter, byte &reg1)
{
    reg1 = memory.read(programCounter);
    programCounter += 0x01;
    return 8;
}

inline cycle LD(Memory &memory, address &programCounter, const byte regHigh, const byte regLow)
{
    memory.write(Gahood::addressFromBytes(regHigh, regLow), memory.read(programCounter));
    programCounter += 0x01;
    return 12;
}

inline cycle LD(Memory &memory, address &programCounter, const byte regHigh, const byte regLow, const byte value)
{
	memory.write(Gahood::addressFromBytes(regHigh, regLow), value);
	return 8;
}

inline cycle LD16(Memory &memory, address &programCounter, byte &regHigh, byte &regLow)
{
    regHigh = memory.read(programCounter + 0x01);
    regLow = memory.read(programCounter);
    programCounter += 0x02;
    return 12;
}

inline cycle LDH(Memory &memory, address &programCounter, byte &regA)
{
	memory.write(Gahood::addressFromBytes(0xFF, memory.read(programCounter)), regA);
	programCounter += 0x01;
	return 12;
}

inline cycle LDH(address &programCounter, byte &regA, const byte value)
{
	regA = value;
	programCounter += 0x01;
	return 12;
}

inline cycle POP(const Memory &memory, address &stackPointer, byte &highReg, byte &lowReg)
{
	lowReg = memory.read(stackPointer);
	stackPointer += 0x01;
	highReg = memory.read(stackPointer);
	stackPointer += 0x01;
	return 12;
}

inline cycle PUSH(Memory &memory, address &stackPointer, const byte highReg, const byte lowReg)
{
	stackPointer -= 0x01;
	memory.write(stackPointer, highReg);
	stackPointer -= 0x01;
	memory.write(stackPointer, lowReg);
	return 16;
}

/* Arithmetic */
inline cycle INC(address &programCounter, byte &flags, byte &reg)
{
    setHalfCarryFlag(flags, (reg & 0x0F) == 0x0F);
    reg += 0x01;
    setSubtractFlag(flags, false);
    setZeroFlag(flags, reg == 0x00);
    return 4;
}

inline cycle INC(Memory &memory, address &programCounter, byte &flags, const byte regHigh, const byte regLow)
{
    const address addr = Gahood::addressFromBytes(regHigh, regLow);
    byte byteToInc = memory.read(addr);
    setHalfCarryFlag(flags, (byteToInc & 0x0F) == 0x0F);
    byteToInc +=  0x01;
    setSubtractFlag(flags, false);
    setZeroFlag(flags, byteToInc == 0x00);
    memory.write(addr, byteToInc);
    return 12;
}

inline cycle INC16(address &programCounter, byte &regHigh, byte &regLow)
{
    const address addr = Gahood::addressFromBytes(regHigh, regLow) + 0x01;
    regHigh = static_cast<byte> (addr >> 8);
    regLow = static_cast<byte> (addr & 0x00FF);
    return 8;
}

inline cycle INC16(address &programCounter, address &stackPointer)
{
    stackPointer += 0x01;
    return 8;
}

inline cycle DEC(address &programCounter, byte &flags, byte &reg)
{
    setHalfCarryFlag(flags, (reg & 0x0F) == 0x00);
    reg -= 0x01;
    setSubtractFlag(flags, true);
    setZeroFlag(flags, reg == 0x00);
    return 4;
}

inline cycle DEC(Memory &memory, address &programCounter, byte &flags, const byte regHigh, const byte regLow)
{
    const address addr = Gahood::addressFromBytes(regHigh, regLow);
    byte byteToDec = memory.read(addr);
    setHalfCarryFlag(flags, (byteToDec & 0x0F) == 0x00);
    byteToDec -= 0x01;
    setSubtractFlag(flags, true);
    setZeroFlag(flags, byteToDec == 0x00);
    memory.write(addr, byteToDec);
    return 12;
}

inline cycle DEC16(address &programCounter, byte &regHigh, byte &regLow)
{
	const address value = Gahood::addressFromBytes(regHigh, regLow) - 0x01;
	regHigh = static_cast<byte> (value >> 8);
	regLow = static_cast<byte> (value & 0x00FF);
	return 8;
}

inline cycle ADD(byte &flags, byte &reg1, const byte reg2)
{
	setHalfCarryFlag(flags, 0x0F - (reg1 & 0x0F) <  (reg2 & 0x0F));
	setCarryFlag(flags, (0xFF - reg1) < reg2);

	reg1 += reg2;
	setZeroFlag(flags, reg1 == 0x00);
	setSubtractFlag(flags, false);

	return 4;
}

inline cycle ADC(byte &flags, byte &reg1, const byte reg2)
{
	const byte carry = getCarryFlag(flags) ? 0x01 : 0x00;
	setHalfCarryFlag(flags, 0x0F - (reg1 & 0x0F) <  ((reg2 + carry) & 0x0F));
	setCarryFlag(flags, (0xFF - reg1) < (reg2 + carry));
	reg1 += reg2;
	reg1 += carry;

	setZeroFlag(flags, reg1 == 0x00);
	setSubtractFlag(flags, false);

	return 4;
}

inline cycle ADD16(address &programCounter, byte &flags, byte &regHigh, byte &regLow, const byte addHigh, const byte addLow)
{
    setHalfCarryFlag(flags, (regLow & 0x0F) + (addLow & 0x0F) > 0x0F);
    setCarryFlag(flags, (0xFF - regLow) < addLow);
    setSubtractFlag(flags, false);
    const address sum = Gahood::addressFromBytes(regHigh, regLow) + Gahood::addressFromBytes(addHigh, addLow);
    regHigh = static_cast<byte> (sum >> 8);
	regLow = static_cast<byte> (sum & 0x00FF);
    return 8;
}

inline cycle SUB(byte &flags, byte &reg1, const byte reg2)
{
	setSubtractFlag(flags, true);
	setHalfCarryFlag(flags, (reg1 & 0x0F) < (reg2 & 0x0F));
	setCarryFlag(flags, reg1 < reg2);
	reg1 -= reg2;
	setZeroFlag(flags, reg1 == 0x00);
	return 4;
}

inline cycle SBC(byte &flags, byte &reg1, const byte reg2)
{
	return SUB(flags, reg1, reg2 + (getCarryFlag(flags) ? 0x01 : 0x00));
}

inline cycle AND(byte &flags, byte &reg1, const byte reg2)
{
	reg1 &= reg2;
	setCarryFlag(flags, false);
	setHalfCarryFlag(flags, true);
	setSubtractFlag(flags, false);
	setZeroFlag(flags, reg1 == 0x00);
	return 4;
}

inline cycle AND(const Memory &memory, address &programCounter, byte &flags, byte &regA)
{
	const byte value = memory.read(programCounter);
	programCounter += 0x01;
	return AND(flags, regA, value) * 2;
}

inline cycle OR(byte &flags, byte &regA, const byte reg)
{
	regA |= reg;
	setZeroFlag(flags, (regA == 0x00));
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setCarryFlag(flags, false);
	return 4;
}

inline cycle XOR(byte &flags, byte &regA, const byte value)
{
	regA ^= value;
	setCarryFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setSubtractFlag(flags, false);
	setZeroFlag(flags, regA == 0x00);
	return 4;
}

inline cycle CP(byte &flags, const byte regA, const byte cpValue)
{
	setZeroFlag(flags, regA == cpValue);
	setSubtractFlag(flags, true);
	setHalfCarryFlag(flags, (regA & 0x0F) < (cpValue & 0x0F));
	setCarryFlag(flags, regA < cpValue);
	return 4;
}

inline cycle CP(Memory &memory, address &programCounter, byte &flags, const byte regA)
{
	const byte cpValue = memory.read(programCounter);
	setZeroFlag(flags, regA == cpValue);
	setSubtractFlag(flags, true);
	setHalfCarryFlag(flags, (regA & 0x0F) < (cpValue & 0x0F));
	setCarryFlag(flags, regA < cpValue);
	programCounter += 0x01;
	return 8;
}

inline cycle CP(Memory &memory, address &programCounter, byte &flags, const byte regA, const byte regHigh, const byte regLow)
{
	const byte cpValue = memory.read(Gahood::addressFromBytes(regHigh, regLow));
	setZeroFlag(flags, regA == cpValue);
	setSubtractFlag(flags, true);
	setHalfCarryFlag(flags, (regA & 0x0F) < (cpValue & 0x0F));
	setCarryFlag(flags, regA < cpValue);
	return 8;
}

inline cycle CPL(byte &flags, byte &regA)
{
	setSubtractFlag(flags, true);
	setHalfCarryFlag(flags, true);
	regA = ~regA;
	return 4;
}

inline cycle DAA(byte &flags, byte &regA)
{
	signed short int a = regA;
	if(getSubtractFlag(flags))
	{
		if (getHalfCarryFlag(flags))
		{
			a = (a - 0x06) & 0xFF;
		}
		if (getCarryFlag(flags))
		{
			a -= 0x60;
		}
	}
	else
	{
		if (getHalfCarryFlag(flags) || ((a & 0x0F) > 0x09))
		{
			a += 0x06;
		}
		if (getCarryFlag(flags) || (a > 0x9F))
		{
			a += 0x60;
		}
	}
	
	setCarryFlag(flags, ((a & 0x0100) == 0x0100));
	setHalfCarryFlag(flags, false);
	regA = static_cast<byte> (a &= 0xFF);
	setZeroFlag(flags, regA == 0x00);
	return 4;
}

inline cycle SCF(byte &flags)
{
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setCarryFlag(flags, true);
	return 4;
}

inline cycle CCF(byte &flags)
{
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setCarryFlag(flags, !getCarryFlag(flags));
	return 4;
}

/* Rotation / Shifts */
inline cycle RLA(byte &flags, byte &regA)
{
    const bool carry = (regA & 0x80) == 0x80;
    regA <<= 1;
    regA = carry ? regA | 0x01 : regA;
    setCarryFlag(flags, carry);
    setZeroFlag(flags, false);
    setSubtractFlag(flags, false);
    setHalfCarryFlag(flags, false);
    return 4;
}

inline cycle RLCA(byte &flags, byte &regA)
{
    const bool carry = (regA & 0x80) == 0x80;
    regA <<= 1;
    regA = getCarryFlag(flags) ? regA | 0x01 : regA;
    setCarryFlag(flags, carry);
    setZeroFlag(flags, false);
    setSubtractFlag(flags, false);
    setHalfCarryFlag(flags, false);
    return 4;
}

inline cycle RRA(byte &flags, byte &regA)
{
	const bool carry = (regA & 0x01) == 0x01;
	regA >>= 1;
	regA = carry ? regA | 0x80 : regA;
	setCarryFlag(flags, carry);
	setZeroFlag(flags, false);
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	return 4;
}

inline cycle RRCA(byte &flags, byte &regA)
{
	const bool carry = (regA & 0x01) == 0x01;
	regA >>= 1;
	regA = getCarryFlag(flags) ? regA | 0x80 : regA;
	setCarryFlag(flags, carry);
	setZeroFlag(flags, false);
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	return 4;
}

/* Jumpers */
inline cycle JP(Memory &memory, address &programCounter)
{
	programCounter = Gahood::addressFromBytes(memory.read(programCounter + 0x01), memory.read(programCounter));
	return 16;
}

inline cycle JP(Memory &memory, address &programCounter, const bool condition)
{
	if (condition)
	{
		return JP(memory, programCounter);
	}
	else
	{
		programCounter += 0x02;
		return 12;
	}
}

inline cycle JP(address &programCounter, const byte regHigh, const byte regLow)
{
	programCounter = Gahood::addressFromBytes(regHigh, regLow);
	return 4;
}

inline cycle JR(Memory &memory, address &programCounter, const bool condition)
{
	if (condition)
	{
		programCounter += static_cast<signed char> (memory.read(programCounter)) + 0x01;
		return 12;
	}
	else
	{
		programCounter += 0x01;
		return 8;
	}
}

inline cycle CALL(Memory &memory, address &programCounter, address &stackPointer)
{
	const address nextProgramCounter = Gahood::addressFromBytes(memory.read(programCounter + 0x01), memory.read(programCounter));
	programCounter += 0x02;
	stackPointer -= 0x01;
	memory.write(stackPointer, static_cast<byte> (programCounter >> 8));
	stackPointer -= 0x01;
	memory.write(stackPointer, static_cast<byte> (programCounter & 0x00FF));
	programCounter = nextProgramCounter;
	return 12;
}

inline cycle CALL(Memory &memory, address &programCounter, address &stackPointer, const bool condition)
{
	if(condition)
	{
		return CALL(memory, programCounter, stackPointer);
	}
	else
	{
		programCounter += 0x02;
		return 12;
	}
	
}

inline cycle RET(Memory &memory, address &programCounter, address &stackPointer)
{
	const byte lowReg = memory.read(stackPointer);
	stackPointer += 0x01;
	const byte highReg = memory.read(stackPointer);
	stackPointer += 0x01;
	programCounter = Gahood::addressFromBytes(highReg, lowReg);
	return 16;
}

inline cycle RST(Memory &memory, address &programCounter, address &stackPointer, const byte resetTo)
{
	stackPointer -= 0x01;
	memory.write(stackPointer, static_cast<byte> ((programCounter & 0xFF00) >> 8));
	stackPointer -= 0x01;
	memory.write(stackPointer, static_cast<byte> (programCounter & 0x00FF));
	programCounter = Gahood::addressFromBytes(0x00, resetTo);
	return 16;
}

/* Flag getters and setters */
inline void setCarryFlag(byte &flags, const bool on)
{
    if(on)
    {
        flags |= 0x10;
    }
    else
    {
        flags &= 0xE0;
    }
}

inline void setHalfCarryFlag(byte &flags, const bool on)
{
    if(on)
    {
        flags |= 0x20;
    }
    else
    {
        flags &= 0xD0;
    }
}

inline void setSubtractFlag(byte &flags, const bool on)
{
    if(on)
    {
        flags |= 0x40;
    }
    else
    {
        flags &= 0xB0;
    }
}

inline void setZeroFlag(byte &flags, const bool on)
{
    if(on)
    {
        flags |= 0x80;
    }
    else
    {
        flags &= 0x70;
    }
}

inline bool getZeroFlag(const byte flags)
{
	return (flags & 0x80) == 0x80;
}

inline bool getSubtractFlag(const byte flags)
{
	return (flags & 0x40) == 0x40;
}

inline bool getHalfCarryFlag(const byte flags)
{
	return (flags & 0x20) == 0x20;
}

inline bool getCarryFlag(const byte flags)
{
	return (flags & 0x10) == 0x10;
}

#endif