#include "util.hpp"
#include <sys/time.h>

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
        Gahood::criticalSdlError("Failed to read file: %s");
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
                Gahood::criticalSdlError("Failed to allocate memory for file read");
            }
            fileBytes = tmp;
        }
    }
    SDL_RWclose(fileCtx);

    return fileBytes;
}

void Gahood::writeToFile(const char *filePath, const byte *bytesToWrite, const size sizeOfBytes)
{
    SDL_RWops *fileCtx = SDL_RWFromFile(filePath, "wb");
    if(!fileCtx)
    {
        Gahood::criticalSdlError("Failed to open file to write to: %s", filePath);
    }

    if(SDL_RWwrite(fileCtx, bytesToWrite, 1, sizeOfBytes) != sizeOfBytes)
    {
        Gahood::criticalSdlError("Failed to write to file");
    }
    SDL_RWclose(fileCtx);
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

byte Gahood::sub(const byte byte1, const byte byte2)
{
    if(byte1 < byte2)
    {
        return 0x00;
    }
    else
    {
        return byte1 - byte2;
    }
}

byte Gahood::add(const byte byte1, const byte byte2)
{
    if(0xFF - byte1 < byte2)
    {
        return 0xFF;
    }
    else
    {
        return byte1 + byte2;
    }
}

address Gahood::sub(const address addr1, const address addr2)
{
    if(addr1 < addr2)
    {
        return 0x0000;
    }
    else
    {
        return addr1 - addr2;
    }
}

address Gahood::add(const address addr1, const address addr2)
{
    if(0xFFFF - addr1 < addr2)
    {
        return 0xFFFF;
    }
    else
    {
        return addr1 + addr2;
    }
}

microseconds Gahood::getCurrentMicroseconds()
{
    timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * static_cast<microseconds> (1000000) + tv.tv_usec;
}

milliseconds Gahood::getCurrentMilliseconds()
{
    return SDL_GetTicks();
}