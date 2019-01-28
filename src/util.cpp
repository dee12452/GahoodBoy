#include "util.hpp"

bool Gahood::stringEquals(char *str1, char *str2)
{
    size iterator = 0;
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

bool Gahood::stringLiteralEquals(char *str1, const char *str2)
{
    size iterator = 0;
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

address Gahood::addressFromBytes(const byte highByte, const byte lowByte)
{
    address twoBytes = highByte;
    twoBytes <<= 8;
    twoBytes |= lowByte;
    return twoBytes;
}

byte * Gahood::readFileAsBytes(const char *filePath, size &sizeOfFile)
{
    SDL_RWops *fileCtx = SDL_RWFromFile(filePath, "rb");
    if(!fileCtx)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to read file: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

	size currentMemorySize = 256; // start with 256 bytes allocated for memory
    byte * fileBytes = (byte *) malloc(sizeof(byte) * currentMemorySize);
    sizeOfFile = 0;
    byte byteBuffer[1];
    while(SDL_RWread(fileCtx, byteBuffer, sizeof(byte), 1))
    {
        fileBytes[sizeOfFile] = byteBuffer[0];
        sizeOfFile++;
        if(sizeOfFile + 1 == currentMemorySize)
        {
            currentMemorySize *= 2;
            byte *tmp = (byte *) realloc(fileBytes, sizeof(byte) * currentMemorySize);
            if(!tmp)
            {
                SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to allocate memory for file read");
                exit(EXIT_FAILURE);
            }
            fileBytes = tmp;
        }
    }
    return fileBytes;
}

void Gahood::log(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, message, args);
    va_end(args);
}

void Gahood::criticalError(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, message, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

void Gahood::criticalSdlError(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, message, args);
    va_end(args);
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL Error: %s", SDL_GetError());
    exit(EXIT_FAILURE);
}