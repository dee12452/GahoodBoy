#ifndef _GAHOOD_BOY_OPCODE_HPP_
#define _GAHOOD_BOY_OPCODE_HPP_

#include "util.hpp"

static void setCarryFlag(uint8_t &flags, const bool on);
static void setHalfCarryFlag(uint8_t &flags, const bool on);
static void setSubtractFlag(uint8_t &flags, const bool on);
static void setZeroFlag(uint8_t &flags, const bool on);

/* Control Instructions */
inline uint8_t NOP(uint16_t &programCounter)
{
    programCounter += 0x01;
    return 4;
}

/* Jumpers :) */
inline uint8_t JP(uint16_t &programCounter, const uint16_t newAddress)
{
    programCounter = newAddress;
    return 16;
}

inline uint8_t JR(uint16_t &programCounter, const signed char addToCounter)
{
    programCounter += addToCounter;
    return 12;
}

inline uint8_t CALL(uint16_t &programCounter, const uint16_t newAddress)
{
    programCounter = newAddress;
    return 24;
}

/* Load, store, move */
inline uint8_t LD(uint16_t &programCounter, uint8_t &reg, const uint8_t value, const bool fromAddress)
{
    reg = value;
    programCounter += 0x01;
    return fromAddress ? 8 : 4;
}

inline uint8_t LD(uint16_t &programCounter, uint16_t &reg, const uint16_t value)
{
    reg = value;
    programCounter += 0x01;
    return 12;
}

inline uint8_t LD(uint16_t &programCounter, uint8_t &highReg, uint8_t &lowReg, const uint8_t highData, const uint8_t lowData)
{
    highReg = highData;
    lowReg = lowData;
    programCounter += 0x03;
    return 12;
}

/* Arithmetic */
inline uint8_t INC(uint16_t &programCounter, uint8_t &flags, uint8_t &reg)
{
    setSubtractFlag(flags, false);
    setHalfCarryFlag(flags, (reg & 0x07) == 0x07);
    reg += 0x01;
    setZeroFlag(flags, reg == 0x00);
    programCounter += 0x01;
    return 4;
}

inline uint8_t INC(uint16_t &programCounter, uint8_t &flags, uint8_t &regHigh, uint8_t &regLow)
{
    uint16_t value = create16Bit(regHigh, regLow);
    value++;
    regHigh = static_cast<uint8_t> (value & 0xFF00 >> 8);
    regLow = static_cast<uint8_t> (value & 0x00FF);
    programCounter += 0x01;
    return 8;
}

inline uint8_t INC(uint16_t &programCounter, uint16_t &reg)
{
    reg++;
    programCounter += 0x01;
    return 8;
}

inline uint8_t DEC(uint16_t &programCounter, uint8_t &flags, uint8_t &reg)
{
    setSubtractFlag(flags, true);
    setHalfCarryFlag(flags, (reg & 0x07) == 0x00);
    reg -= 0x01;
    setZeroFlag(flags, reg == 0x00);
    programCounter += 0x01;
    return 4;
}

inline uint8_t SUB(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value, const bool fromAddress)
{
    setSubtractFlag(flags, true);
    registerA -= value;
    setHalfCarryFlag(flags, (value << 4) > (registerA << 4));
    setCarryFlag(flags, (value > registerA));
    registerA -= value;
    setZeroFlag(flags, registerA == 0x00);

    programCounter += 0x01;
    return fromAddress ? 8 : 4;
}

inline uint8_t SBC(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA, const uint8_t value, const bool fromAddress)
{
    const bool carryOn = (flags & 0x10) == 0x10;
    uint8_t valueToSubtract = carryOn ? value + 1 : value; // Add 1 to value if carry flag is on
    return SUB(programCounter, flags, registerA, valueToSubtract, fromAddress);
}

inline uint8_t CPL(uint16_t &programCounter, uint8_t &flags, uint8_t &registerA)
{
    setSubtractFlag(flags, true);
    setHalfCarryFlag(flags, true);
    registerA = ~registerA;
    programCounter += 0x01;
    return 4;
}

/* Rotation / Shifts */


static void setCarryFlag(uint8_t &flags, const bool on)
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

static void setHalfCarryFlag(uint8_t &flags, const bool on)
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

static void setSubtractFlag(uint8_t &flags, const bool on)
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

static void setZeroFlag(uint8_t &flags, const bool on)
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

#endif