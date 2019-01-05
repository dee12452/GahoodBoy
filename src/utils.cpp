#include "utils.hpp"
#include <stddef.h>

bool string_equals(char *str1, char *str2)
{
    size_t iterator = 0;
    while(str1[iterator] == str2[iterator])
    {
        if(str1[iterator] == '\0')
        {
            return true;
        }
        iterator++;
    }
    return false;
}

bool string_literal_equals(char *str1, const char *str2)
{
    size_t iterator = 0;
    while(str1[iterator] == str2[iterator])
    {
        if(str1[iterator] == '\0')
        {
            return true;
        }
        iterator++;
    }
    return false;
}

uint16_t create16Bit(const uint8_t byte1, const uint8_t byte2)
{
    uint16_t twoBytes = byte1;
    twoBytes <<= 8;
    twoBytes |= byte2;
    return twoBytes;
}