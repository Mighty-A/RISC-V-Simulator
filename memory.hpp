//
// Created by yhy79 on 2020/7/6.
//

#ifndef RISC_V_MEMORY_HPP
#define RISC_V_MEMORY_HPP

#include "type.hpp"
const int MAX_MEMORY_SIZE = 0xfffff;

class Memory {
public:
    Byte mem[MAX_MEMORY_SIZE];

    Byte LoadByte(int address) {
        return mem[address];
    }

    HalfWord LoadHalfWord(int address) {
        return HalfWord(mem[address + 1].b, mem[address].b);
    }

    Word LoadWord(int address) {
        return Word(mem[address + 3].b, mem[address + 2 ].b, mem[address + 1].b, mem[address].b);
    }

    void StoreByte(int address, Byte byte) {
        mem[address] = byte;
    }

    void StoreHalfWord(int address, HalfWord halfWord) {
        mem[address + 1] = halfWord.b.b1;
        mem[address] = halfWord.b.b2;
    }

    void StoreWord(int address, Word word) {
        mem[address + 3] = word.b.b1;
        mem[address + 2] = word.b.b2;
        mem[address + 1] = word.b.b3;
        mem[address] = word.b.b4;
    }
} memory;
#endif //RISC_V_MEMORY_HPP
