#ifndef _GAHOOD_BOY_OPCODE_PREFIX_HPP_
#define _GAHOOD_BOY_OPCODE_PREFIX_HPP_

#include "opcode.hpp"

inline cycle RLC(byte &flags, byte &reg)
{
	const bool carry = (reg & 0x80) == 0x80;
	reg <<= 1;
	reg += carry ? 0x01 : 0x00;
	setZeroFlag(flags, reg == 0x00);
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setCarryFlag(flags, carry);
	return 8;
}

inline cycle RL(byte &flags, byte &reg)
{
	const bool carry = (reg & 0x80) == 0x80;
	reg <<= 1;
	reg += getCarryFlag(flags) ? 0x01 : 0x00;
	setZeroFlag(flags, reg == 0x00);
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setCarryFlag(flags, carry);
	return 8;
}

inline cycle RRC(byte &flags, byte &reg)
{
	const bool carry = (reg & 0x01) == 0x01;
	reg >>= 1;
	reg |= carry ? 0x80 : 0x00;
	setZeroFlag(flags, reg == 0x00);
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setCarryFlag(flags, carry);
	return 8;
}

inline cycle RR(byte &flags, byte &reg)
{
	const bool carry = (reg & 0x01) == 0x01;
	reg >>= 1;
	reg |= getCarryFlag(flags) ? 0x80 : 0x00;
	setZeroFlag(flags, reg == 0x00);
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setCarryFlag(flags, carry);
	return 8;
}

inline cycle SLA(byte &flags, byte &reg)
{
	setCarryFlag(flags, (reg & 0x80) == 0x80);
	reg <<= 1;
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setZeroFlag(flags, reg == 0x00);
	return 8;
}

inline cycle SRA(byte &flags, byte &reg)
{
	reg >>= 1;
	setCarryFlag(flags, false);
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setZeroFlag(flags, reg == 0x00);
	return 8;
}

inline cycle SWAP(byte &flags, byte &reg)
{
	const byte lowerNibleToUpper = (0x0F & reg) << 4; // Get lower middle and move to upper
	reg >>= 4; // Move upper nible to lower
	reg |= lowerNibleToUpper; // Put the lower nibles back into the upper nible of the register
	setZeroFlag(flags, reg == 0);
	setCarryFlag(flags, false);
	setHalfCarryFlag(flags, false);
	setSubtractFlag(flags, false);
	return 8;
}

inline cycle SRL(byte &flags, byte &reg)
{
	setCarryFlag(flags, (reg & 0x01) == 0x01);
	reg >>= 1;
	setZeroFlag(flags, reg == 0x00);
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, false);
	return 8;
}

inline cycle BIT(byte &flags, const byte reg, const BitNumber bitNumber)
{
	setZeroFlag(flags, !Gahood::bitOn(reg, bitNumber));
	setSubtractFlag(flags, false);
	setHalfCarryFlag(flags, true);
	return 8;
}

inline cycle RES(byte &reg, const BitNumber bitNumber)
{
	reg &= (~(1 << bitNumber));
	return 8;
}

inline cycle SET(byte &reg, const BitNumber bitNumber)
{
	reg |= (1 << bitNumber);
	return 8;
}

#endif