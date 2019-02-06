#ifndef _GAHOOD_BOY_UTIL_HPP_
#define _GAHOOD_BOY_UTIL_HPP_

#include <SDL2/SDL.h>
#include "const.hpp"

#ifndef EXIT_FAILURE
#include <cstdlib>
#endif

typedef short int cycle;
typedef unsigned char byte;
typedef unsigned short int address;
typedef unsigned long size;

namespace Gahood 
{
    bool stringEquals(char *str1, char *str2);
    bool stringLiteralEquals(char *str1, const char *str2);
    address addressFromBytes(const byte highByte, const byte lowByte);
    byte * readFileAsBytes(const char *filePath, size &sizeOfFile);
    void writeToFile(const char *filePath, const byte *bytesToWrite, const size sizeOfBytes);
    void log(const char *message, ...);
    void criticalError(const char *message, ...);
    void criticalSdlError(const char *message, ...);
    byte sub(const byte byte1, const byte byte2);
    byte add(const byte byte1, const byte byte2);
    address sub(const address addr1, const address addr2);
    address add(const address addr1, const address addr2);
}

#endif