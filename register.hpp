//
// Created by yhy79 on 2020/7/6.
//

#ifndef RISC_V_REGISTER_HPP
#define RISC_V_REGISTER_HPP

#include "type.hpp"
#include <cstring>

class Register {
public:
    Word reg[32];
    Word pc;
    bool used[32];
    bool jumpFlag = false;
    Register() {
        memset(reg, 0, sizeof(Word) * 32);
        pc.u_num = 0;
        memset(used, 0, sizeof(bool) * 32);
    }
    Word Getpc() {
        return pc;
    }
    Word GetReg(unsigned int rd) {
        return reg[rd];
    }
    void Setpc(Word t_pc) {
        this->pc = t_pc;
    }
    void SetReg(unsigned int rd, Word word) {
        if (rd != 0)
            reg[rd] = word;
    }
} Register;


#endif //RISC_V_REGISTER_HPP
