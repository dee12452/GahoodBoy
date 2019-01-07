#ifndef _GAHOOD_BOY_UTIL_HPP_
#define _GAHOOD_BOY_UTIL_HPP_

#include <stdint.h>

bool stringEquals(char *str1, char *str2);
bool stringLiteralEquals(char *str1, const char *str2);
uint16_t create16Bit(const uint8_t byte1, const uint8_t byte2);

#endif