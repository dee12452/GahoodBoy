#ifndef _GAHOOD_BOY_OPCODE_HPP_
#define _GAHOOD_BOY_OPCODE_HPP_

#include "memory.hpp"

inline bool getCarryFlag(byte &flags);
inline void setCarryFlag(byte &flags, const bool on);
inline void setHalfCarryFlag(byte &flags, const bool on);
inline void setSubtractFlag(byte &flags, const bool on);
inline void setZeroFlag(byte &flags, const bool on);
inline bool getZeroFlag(byte &flags);

/* Control Instructions */
inline cycle NOP(address &programCounter)
{
    programCounter += 0x01;
    return 4;
}

inline cycle DI(address &programCounter, bool &IME)
{
    IME = false;
	programCounter += 0x01;
	return 4;
}

inline cycle EI(address &programCounter, bool &IME)
{
    IME = true;
	programCounter += 0x01;
	return 4;
}

/* Load, store, move */
inline cycle LD(address &programCounter, byte &reg1, const byte reg2)
{
    reg1 = reg2;
    programCounter += 0x01;
    return 4;
}

inline cycle LD(Memory &memory, address &programCounter, byte &reg1)
{
    reg1 = memory.read(programCounter + 0x01);
    programCounter += 0x02;
    return 8;
}

inline cycle LD(Memory &memory, address &programCounter, const byte regHigh, const byte regLow)
{
    memory.write(Gahood::addressFromBytes(regHigh, regLow), memory.read(programCounter + 0x01));
    programCounter += 0x02;
    return 12;
}

inline cycle LD(Memory &memory, address &programCounter, const byte regHigh, const byte regLow, const byte value)
{
	memory.write(Gahood::addressFromBytes(regHigh, regLow), value);
	programCounter += 0x01;
	return 8;
}

inline cycle LD16(Memory &memory, address &programCounter, byte &regHigh, byte &regLow)
{
    regHigh = memory.read(programCounter + 0x01);
    regLow = memory.read(programCounter + 0x02);
    programCounter += 0x03;
    return 12;
}

inline cycle LD16(Memory &memory, address &programCounter, address &stackPointer, bool intoStackPointer)
{
    if(intoStackPointer)
    {
        stackPointer = Gahood::addressFromBytes(memory.read(programCounter + 0x01), memory.read(programCounter + 0x02));
        programCounter += 0x03;
        return 12;
    }
    else
    {
        const address addrToPut = Gahood::addressFromBytes(memory.read(programCounter + 0x01), memory.read(programCounter + 0x02));
        memory.write(addrToPut, static_cast<byte> (stackPointer >> 8));
        memory.write(addrToPut + 0x01, static_cast<byte> (stackPointer & 0x0F));
        programCounter += 0x03;
        return 20;
    }
}

inline cycle LDH(Memory &memory, address &programCounter, byte &regA)
{
	memory.write(Gahood::addressFromBytes(0xFF, memory.read(programCounter + 0x01)), regA);
	programCounter += 0x02;
	return 12;
}

inline cycle LDH(address &programCounter, byte &regA, const byte value)
{
	regA = value;
	programCounter += 0x02;
	return 12;
}

/* Arithmetic */
inline cycle INC(address &programCounter, byte &flags, byte &reg)
{
    setHalfCarryFlag(flags, (reg & 0x0F) == 0x0F);
    reg += 0x01;
    setSubtractFlag(flags, false);
    setZeroFlag(flags, reg == 0x00);
    programCounter += 0x01;
    return 4;
}

inline cycle INC(Memory &memory, address &programCounter, byte &flags, const byte regHigh, const byte regLow)
{
    const address addr = Gahood::addressFromBytes(regHigh, regLow);
    byte byteToInc = memory.read(addr);
    setHalfCarryFlag(flags, (byteToInc & 0x0F) == 0x0F);
    byteToInc += 0x01;
    setSubtractFlag(flags, false);
    setZeroFlag(flags, byteToInc == 0x00);
    memory.write(addr, byteToInc);
    programCounter += 0x01;
    return 12;
}

inline cycle INC16(address &programCounter, byte &regHigh, byte &regLow)
{
    const address addr = Gahood::addressFromBytes(regHigh, regLow) + 0x01;
    regHigh = static_cast<byte> (addr >> 8);
    regLow = static_cast<byte> (addr & 0x00FF);
    programCounter += 0x01;
    return 8;
}

inline cycle INC16(address &programCounter, address &stackPointer)
{
    stackPointer += 0x01;
    programCounter += 0x01;
    return 8;
}

inline cycle DEC(address &programCounter, byte &flags, byte &reg)
{
    setHalfCarryFlag(flags, (reg & 0x0F) == 0x00);
    reg = Gahood::sub(reg, 0x01);
    setSubtractFlag(flags, true);
    setZeroFlag(flags, reg == 0x00);
    programCounter += 0x01;
    return 4;
}

inline cycle DEC(Memory &memory, address &programCounter, byte &flags, const byte regHigh, const byte regLow)
{
    const address addr = Gahood::addressFromBytes(regHigh, regLow);
    byte byteToDec = memory.read(addr);
    setHalfCarryFlag(flags, (byteToDec & 0x0F) == 0x00);
    byteToDec = Gahood::sub(byteToDec, 0x01);
    setSubtractFlag(flags, true);
    setZeroFlag(flags, byteToDec == 0x00);
    memory.write(addr, byteToDec);
    programCounter += 0x01;
    return 12;
}

inline cycle DEC16(address &programCounter, byte &regHigh, byte &regLow)
{
	const address value = Gahood::sub(Gahood::addressFromBytes(regHigh, regLow), 0x01);
	regHigh = static_cast<byte> ((value & 0xFF00) >> 8);
	regLow = static_cast<byte> (value & 0x00FF);
	programCounter += 0x01;
	return 8;
}

inline cycle ADD16(address &programCounter, byte &flags, byte &regHigh, byte &regLow, const byte addHigh, const byte addLow)
{
    setHalfCarryFlag(flags, (regLow & 0x0F) + (addLow & 0x0F) > 0x0F);
    setCarryFlag(flags, ((regLow >> 4) & 0x0F) + ((addLow >> 4) & 0x0F) > 0x0F);
    setSubtractFlag(flags, false);
    const address sum = Gahood::addressFromBytes(regHigh, regLow) + Gahood::addressFromBytes(addHigh, addLow);
    regLow = static_cast<byte> (sum & 0x00FF);
    regHigh = static_cast<byte> (sum >> 8);
    return 8;
}

inline cycle OR(address &programCounter, byte &flags, byte &regA, const byte reg)
{
	regA |= reg;
	setZeroFlag(flags, (regA == 0x00));
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setCarryFlag(flags, false);
	programCounter += 0x01;
	return 4;
}

inline cycle XOR(address &programCounter, byte &flags, byte &regA, const byte value)
{
	regA ^= value;
	setCarryFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setSubtractFlag(flags, false);
	setZeroFlag(flags, regA == 0x00);
	programCounter += 0x01;
	return 4;
}

inline cycle CP(address &programCounter, byte &flags, const byte regA, const byte cpValue)
{
	setZeroFlag(flags, regA == cpValue);
	setSubtractFlag(flags, true);
	setHalfCarryFlag(flags, (regA & 0x0F) < (cpValue & 0x0F));
	setCarryFlag(flags, regA < cpValue);
	programCounter += 0x01;
	return 4;
}

inline cycle CP(Memory &memory, address &programCounter, byte &flags, const byte regA)
{
	const byte cpValue = memory.read(programCounter + 0x01);
	setZeroFlag(flags, regA == cpValue);
	setSubtractFlag(flags, true);
	setHalfCarryFlag(flags, (regA & 0x0F) < (cpValue & 0x0F));
	setCarryFlag(flags, regA < cpValue);
	programCounter += 0x02;
	return 8;
}

inline cycle CP(Memory &memory, address &programCounter, byte &flags, const byte regA, const byte regHigh, const byte regLow)
{
	const byte cpValue = memory.read(Gahood::addressFromBytes(regHigh, regLow));
	setZeroFlag(flags, regA == cpValue);
	setSubtractFlag(flags, true);
	setHalfCarryFlag(flags, (regA & 0x0F) < (cpValue & 0x0F));
	setCarryFlag(flags, regA < cpValue);
	programCounter += 0x01;
	return 8;
}

/* Rotation / Shifts */
inline cycle RLCA(address &programCounter, byte &flags, byte &regA)
{
    const bool carry = (regA & 0x80) == 0x80;
    regA <<= 1;
    regA = carry ? regA | 0x01 : regA;
    setCarryFlag(flags, carry);
    setZeroFlag(flags, false);
    setSubtractFlag(flags, false);
    setHalfCarryFlag(flags, false);
    programCounter += 0x01;
    return 4;
}

inline cycle RLA(address &programCounter, byte &flags, byte &regA)
{
    const bool carry = (regA & 0x80) == 0x80;
    regA <<= 1;
    regA = getCarryFlag(flags) ? regA | 0x01 : regA;
    setCarryFlag(flags, carry);
    setZeroFlag(flags, false);
    setSubtractFlag(flags, false);
    setHalfCarryFlag(flags, false);
    programCounter += 0x01;
    return 4;
}

/* Jumpers */
inline cycle JP(Memory &memory, address &programCounter)
{
	programCounter = Gahood::addressFromBytes(memory.read(programCounter + 0x02), memory.read(programCounter + 0x01));
	return 16;
}

inline cycle JR(Memory &memory, address &programCounter, const bool condition)
{
	if (condition)
	{
		programCounter += 0x01 + static_cast<signed char> (memory.read(programCounter + 0x01));
		return 12;
	}
	else
	{
		programCounter += 0x02;
		return 8;
	}
}

inline cycle CALL(Memory &memory, address & programCounter, address &stackPointer)
{
	stackPointer = Gahood::sub(stackPointer, 0x01);
	memory.write(stackPointer, static_cast<byte> ((programCounter & 0xFF00) >> 8));
	stackPointer = Gahood::sub(stackPointer, 0x01);
	memory.write(stackPointer, static_cast<byte> (programCounter & 0x00FF));
	programCounter = Gahood::addressFromBytes(memory.read(programCounter + 0x02), memory.read(programCounter + 0x01));
	return 12;
}

inline cycle RET(Memory &memory, address & programCounter, address &stackPointer)
{
	programCounter = Gahood::addressFromBytes(memory.read(stackPointer + 0x02), memory.read(stackPointer + 0x01));
	stackPointer += 0x02;
	return 16;
}

/* Flag getters and setters */
inline bool getCarryFlag(byte &flags)
{
    return (flags & 0x10) == 0x10;
}

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

inline bool getZeroFlag(byte &flags)
{
	return (flags & 0x80) == 0x80;
}

#endif