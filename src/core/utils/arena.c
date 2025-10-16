#pragma once

#include "../types.c"

typedef struct Arena
{
    char m_bytesStack[512 * 1024];
    u32 m_bytesAllocated;
} Arena;

void* ArenaPush(Arena* _stack, const uint32_t _bytes);
void ArenaPop(Arena* _stack, void* _value);
void ArenaSwap(Arena* restrict* _stackA, Arena* restrict* _stackB);

void* ArenaPush(Arena* _stack, const uint32_t _bytes)
{
    char* address = _stack->m_bytesStack + _stack->m_bytesAllocated;
    _stack->m_bytesAllocated += _bytes;
    return (void*)address;
}

void ArenaPop(Arena* _stack, void* _value)
{
    void* currentAddressAvailable = _stack->m_bytesStack + _stack->m_bytesAllocated;
    const uPtr numberDeallocated = (!!(uPtr)_value) * (uPtr)currentAddressAvailable - (uPtr)_value;
    _stack->m_bytesAllocated -= numberDeallocated;
}

void ArenaSwap(Arena* restrict * _stackA, Arena* restrict * _stackB)
{
    Arena* tmp = *_stackA;
    *_stackA = *_stackB;
    *_stackB = tmp;
}