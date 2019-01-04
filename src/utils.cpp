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