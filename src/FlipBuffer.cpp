#include "FlipBuffer.h"

using namespace TapuinoNext;

FlipBuffer::FlipBuffer(uint32_t bufferSize)
{
  // ensure the buffer size is a power of 2
    uint64_t powerTwo = 2;
    while (powerTwo < bufferSize)
    {
        powerTwo <<= 1;
    }
    powerTwo >>= 1;

    this->bufferSize = powerTwo;
    bufferMask = this->bufferSize - 1;
    halfBufferSize = this->bufferSize >> 1;
    bufferSwitchPos = halfBufferSize;

    pBuffer = NULL;
    bufferPos = 0;
}

FlipBuffer::~FlipBuffer()
{
    if (pBuffer != NULL)
    {
        free(pBuffer);
        pBuffer = NULL;
    }
}

ErrorCodes FlipBuffer::Init()
{
    if (pBuffer == NULL)
    {
        pBuffer = (uint8_t*) malloc(bufferSize);
        if (pBuffer == NULL)
            return ErrorCodes::OUT_OF_MEMORY;
    }

    bufferPos = 0;
    return ErrorCodes::OK;
}

void FlipBuffer::Reset()
{
    if (pBuffer != NULL)
    {
        memset(pBuffer, 0, bufferSize);
        bufferPos = 0;
    }
}

ErrorCodes FlipBuffer::SetHeader(uint8_t* header, uint32_t size)
{
    if (size > bufferSize)
    {
        return ErrorCodes::OUT_OF_RANGE;
    }
    
    memcpy(pBuffer, header, size);
    bufferPos = 0;
    return ErrorCodes::OK;
}

ErrorCodes FlipBuffer::FillWholeBuffer(File file, uint32_t atPos)
{
    if (atPos < bufferSize)
    {
        file.read(&pBuffer[atPos], bufferSize - atPos);
        bufferPos = 0;
        return ErrorCodes::OK;
    }
    return ErrorCodes::OUT_OF_RANGE;
}


uint8_t FlipBuffer::ReadByte()
{
    if (bufferPos == bufferSwitchPos)
    {
        bufferSwitchFlag = true;
    }

    uint8_t ret = pBuffer[bufferPos++];
    bufferPos &= bufferMask;
    return ret;
}

void FlipBuffer::FillBufferIfNeeded(File tapFile)
{
    if (bufferSwitchFlag)
    {
        bufferSwitchFlag = false;
        bufferSwitchPos = halfBufferSize - bufferSwitchPos;
        tapFile.readBytes((char*) &pBuffer[bufferSwitchPos], halfBufferSize);
    }
}

void FlipBuffer::WriteByte(uint8_t value)
{
    if (bufferPos == bufferSwitchPos)
    {
        bufferSwitchFlag = true;
    }

    pBuffer[bufferPos++] = value;
    bufferPos &= bufferMask;
}

void FlipBuffer::FlushBufferIfNeeded(File tapFile)
{
    if (bufferSwitchFlag)
    {
        bufferSwitchFlag = false;
        bufferSwitchPos = halfBufferSize - bufferSwitchPos;
        tapFile.write(&pBuffer[bufferSwitchPos], halfBufferSize);
    }
}

void FlipBuffer::FlushBufferFinal(File tapFile)
{
    if (bufferPos < halfBufferSize)
    {
        tapFile.write(pBuffer, bufferPos);
    }
    else
    {
        tapFile.write(&pBuffer[halfBufferSize], bufferPos - halfBufferSize);
    }
}
