#ifndef _GAHOOD_BOY_UTIL_HPP_
#define _GAHOOD_BOY_UTIL_HPP_

#include <SDL2/SDL.h>
#include "const.hpp"

typedef short int cycle;
typedef unsigned char byte;
typedef unsigned short int address;
typedef unsigned long size_t;

namespace Gahood 
{
    bool stringEquals(char *str1, char *str2);
    bool stringLiteralEquals(char *str1, const char *str2);
    address addressFromBytes(const byte highByte, const byte lowByte);
    byte * readFileAsBytes(const char *filePath, size_t &sizeOfFile);
    void log(const char *message, ...);
    void criticalError(const char *message, ...);
    void criticalSdlError(const char *message, ...);
}

#endif