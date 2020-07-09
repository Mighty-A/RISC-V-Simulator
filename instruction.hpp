//
// Created by yhy79 on 2020/7/6.
//

#ifndef RISC_V_INSTRUCTION_HPP
#define RISC_V_INSTRUCTION_HPP

#include "type.hpp"
#include "memory.hpp"
/*
 * Abstract instructions
 */
class AbstractIns {
protected:
    Word _pc; // the current pc
public:
    AbstractIns() {
        _pc = Register.Getpc();
    }
    virtual void Execute() = 0;
    virtual void MemoryAccess() = 0;
    virtual void WriteBack() = 0;
};

class TypeRIns : public AbstractIns {
private:
    int opcode;
    Word rs1, rs2, res;
    int funct7, funct3;
    int rd;
public:
    TypeRIns(int opcode, const Word& rs1, const Word& rs2, int funct3, int funct7, int rd) :
        AbstractIns(),
        opcode(opcode),
        rs1(rs1),
        rs2(rs2),
        funct3(funct3),
        funct7(funct7),
        rd(rd)
    {}

    void Execute() override {               // EXE
        if (funct3 == 0x0 && funct7 == 0x0) {           // ADD
            res.num = rs1.num + rs2.num;
        } else if (funct3 == 0x0 && funct7 == 0x20) {   // SUB
            res.num = rs1.num - rs2.num;
        } else if (funct3 == 0x1 && funct7 == 0x0) {    // Shift Left Logical
            res.u_num = rs1.u_num << (rs2.u_num & 0x1Fu);
        } else if (funct3 == 0x2 && funct7 == 0x0) {    // Set if Less Than
            res.num = (rs1.num < rs2.num) ? 1 : 0;
        } else if (funct3 == 0x3 && funct7 == 0x0) {    // Set if Less Than, Unsigned
            res.num = (rs1.u_num < rs2.u_num) ? 1 : 0;
        } else if (funct3 == 0x4 && funct7 == 0x0) {    // Exclusive-OR
            res.u_num = rs1.u_num ^ rs2.u_num;
        } else if (funct3 == 0x5 && funct7 == 0x0) {    // Shift Right Logical
            res.u_num = rs1.u_num >> (rs2.u_num & 0x1Fu);
        } else if (funct3 == 0x5 && funct7 == 0x20) {   // Shift Right Arithmetic
            res.num = rs1.num >> (rs2.num & 0x1F);              // for signed integer, ">>" is arithmetic
        } else if (funct3 == 0x6 && funct7 == 0x0) {    // OR
            res.u_num = rs1.u_num | rs2.u_num;
        } else if (funct3 == 0x7 && funct7 == 0x0) {    // AND
            res.u_num = rs1.u_num & rs2.u_num;
        } else {
            std::cout << "It's not R type" << std::endl;
        }
    }

    void MemoryAccess() override {          // MEM

    }

    void WriteBack() override {             // WB
        Register.SetReg(rd, res);
    }

};

class TypeIIns : public AbstractIns {
private:
    int opcode;
    Word rs1, imm11_0, res, t;
    int funct3;
    int rd;
    int address;
public:
    TypeIIns(int opcode, const Word& rs1, const Word& imm11_0, int funct3, int rd) :
        AbstractIns(),
        opcode(opcode),
        rs1(rs1),
        imm11_0(imm11_0),
        funct3(funct3),
        rd(rd)
    {}
    void Execute() override {               // EXE
        if (opcode == 0x13) {
            if (funct3 == 0x0) {                        // ADD Immediate
                res.num = rs1.num + imm11_0.num;
            } else if (funct3 == 0x2) {                 // Set if Less Than Immediate
                res.num = (rs1.num < imm11_0.num) ? 1 : 0;
            } else if (funct3 == 0x3) {                 // Set if Less Than Immediate, Unsigned
                res.num = (rs1.u_num < imm11_0.u_num) ? 1 : 0;
            } else if (funct3 == 0x4) {                 // Exclusive-OR, Immediate
                res.num = rs1.num ^ imm11_0.num;
            } else if (funct3 == 0x6) {                 // OR immediate
                res.num = rs1.num | imm11_0.num;
            } else if (funct3 == 0x7) {                 // AND immediate
                res.num = rs1.num & imm11_0.num;
            } else if (funct3 == 0x1) {                 // Shift Left Logical Immediate
                int shamt = imm11_0.num & 0x3F;
                res.num = rs1.num << shamt;
            } else if (funct3 == 0x5) {
                if (imm11_0 >> 11 == 0) {             // Shift Right Logical Immediate
                    unsigned int shamt = imm11_0.u_num & 0x3Fu;
                    res.u_num = rs1.u_num >> shamt;
                } else {                                // Shift Right Arithmetic Immediate
                    int shamt = imm11_0.num & 0x3F;
                    res.num = rs1.num >> shamt;
                }
            }
            // debug
        } else if (opcode == 0x3) {                     // Load
            int offset = (imm11_0.num >> 11 == 1) ? (imm11_0.num | (0xFFFFF << 12)) : imm11_0.num;
            address = rs1.num + offset;
        } else if (opcode == 0x67) {                    // Jump And Link Register
            t.num = _pc.num + 4;
            int offset = (imm11_0.num >> 11 == 1) ? (imm11_0.num | (0xFFFFF << 12)) : imm11_0.num;
            res.num = (rs1.num + offset) & ~1;
        }
    }

    void MemoryAccess() override { // MEM
        if (opcode == 0x13) {         //do nothing
        } else if (opcode == 0x3) {
            if (funct3 == 0x0) {                        // Load Byte
                Byte tmp = memory.LoadByte(address);
                res.u_num = 0;
                res.u_num |= tmp.b;
                if (tmp.b >> 7 == 1) {       // sext
                    res.u_num |= 0xFFFFFF << 8;
                }
            } else if (funct3 == 0x1) {                 // Load HalfWord
                HalfWord tmp = memory.LoadHalfWord(address);
                res.u_num = 0;
                res.u_num |= tmp.u_num;
                if (tmp.u_num >> 15 == 1) {
                    res.u_num |= 0xFFFF << 16;
                }
            } else if (funct3 == 0x2) {                 // Load Word
                Word tmp = memory.LoadWord(address);
                res.u_num = 0;
                res.u_num |= tmp.u_num;
            } else if (funct3 == 0x4) {                 // Load Byte Unsigned
                Byte tmp = memory.LoadByte(address);
                res.u_num = 0;
                res.u_num |= tmp.b;
            } else if (funct3 == 0x5) {                 // Load Half Byte Unsigned
                HalfWord tmp = memory.LoadHalfWord(address);
                res.u_num = 0;
                res.u_num |= tmp.u_num;
            }
        } else if (opcode == 0x67) {            // do nothing

        }
    }

    void WriteBack() override { // WB
        if (opcode == 0x13) {
            Register.SetReg(rd, res);
        } else if (opcode == 0x3) {
            Register.SetReg(rd, res);
        } else if (opcode == 0x67) {
            Register.Setpc(res);
            Register.jumpFlag = true;
            Register.SetReg(rd, t);
        }
    }
};

class TypeSIns : public AbstractIns {
private:
    int opcode;
    Word rs1, rs2, imm, res;
    int rd;
    int funct3;
public:
    TypeSIns(int opcode, const Word& rs1, const Word& rs2, const Word& imm, int funct3) :
        AbstractIns(),
        opcode(opcode),
        rs1(rs1),
        rs2(rs2),
        imm(imm),
        funct3(funct3)
    {}

    void Execute() override {               // EXE
        if (funct3 == 0x0) {                            // Store Byte
            rd = rs1.num + imm.num;
            res.u_num = rs2.u_num & 0xFFu;
        } else if (funct3 == 0x1) {                     // Store HalfWord
            rd = rs1.num + imm.num;
            res.u_num = rs2.u_num & 0xFFFFu;
        } else if (funct3 == 0x2) {                     // Store Word
            rd = rs1.num + imm.num;
            res.u_num = rs2.u_num;
        }
    }

    void MemoryAccess() override {          // MEM
        if (funct3 == 0x0) {                            // Store Byte
            memory.StoreByte(rd, Byte(res.b.b4));
        } else if (funct3 == 0x1) {                     // Store HalfWord
            memory.StoreHalfWord(rd, HalfWord(res.b.b3, res.b.b4));
        } else if (funct3 == 0x2) {                     // Store Word
            memory.StoreWord(rd, res);
        }
    }

    void WriteBack() override {             // WB       // do nothing

    }
};

class TypeBIns : public AbstractIns {
private:
    int opcode;
    Word rs1, rs2, imm;
    int funct3;
    bool flag;
public:
    TypeBIns(int opcode, const Word& rs1, const Word& rs2, const Word& imm, int funct3) :
        AbstractIns(),
        opcode(opcode),
        rs1(rs1),
        rs2(rs2),
        imm(imm),
        funct3(funct3)
    {}

    void Execute() override {               // EXE
        if (funct3 == 0x0) {                            // Branch if Equal
            flag = rs1.num == rs2.num;
        } else if (funct3 == 0x1) {                     // Branch if Not Equal
            flag = (rs1.num != rs2.num);
        } else if (funct3 == 0x4) {                     // Branch if Less Than
            flag = (rs1.num < rs2.num);
        } else if (funct3 == 0x5) {                     // Branch if Greater Than or Equal
            flag = (rs1.num >= rs2.num);
        } else if (funct3 == 0x6) {                     // Branch if Less Than, Unsigned
            flag = (rs1.u_num < rs2.u_num);
        } else if (funct3 == 0x7) {                     // Branch if Greater Than or Equal, Unsigned
            flag = (rs1.u_num >= rs2.u_num);
        }
    }

    void MemoryAccess() override {          // MEM      // do nothing

    }

    void WriteBack() override {             // WB
        if (flag) {
            Register.Setpc(Word(Register.Getpc().num + imm.num));
            Register.jumpFlag = true;
        }
    }
};

class TypeUIns : public AbstractIns {
private:
    int opcode;
    Word imm, res;
    int rd;
public:
    TypeUIns(int opcode, const Word& imm, int rd) :
        AbstractIns(),
        opcode(opcode),
        imm(imm),
        rd(rd)
    {}

    void Execute() override {               // EXE
        if (opcode == 0x37) {                       // Load Upper Immediate
            res.num = (imm.u_num >> 12u) << 12u;
        } else if (opcode == 0x17) {                // Add Upper Immediate to PC
            res.num = _pc.num + (imm.u_num >> 12u) << 12u;
        }
    }

    void MemoryAccess() override {          // MEM

    }

    void WriteBack() override {             // WB
        Register.SetReg(rd, res);
    }

};

class TypeJIns : public AbstractIns {
private:
    int opcode;
    Word imm, res;
    int rd;
public:
    TypeJIns(int opcode, const Word& imm, int rd) :
        AbstractIns(),
        opcode(opcode),
        imm(imm),
        rd(rd)
    {}

    void Execute() override {               // EXE
        if (opcode == 0x6F) {                       // Jump And Link
            res.num = _pc.num + 4;
            _pc.num += imm.num;                 //
        }
    }

    void MemoryAccess() override {          // MEM

    }

    void WriteBack() override {             // WB
        Register.SetReg(rd, res);
        Register.Setpc(_pc);
        Register.jumpFlag = true;
    }
};

Word InstructionFetch(Word pc) {            // IF
    int tmp = int(pc);
    return Word(memory.mem[pc + 3].b, memory.mem[pc + 2].b, memory.mem[pc + 1].b, memory.mem[pc].b);
}

void InstructionDecode(Word ins, AbstractIns *& p) {    // base pointer to the derived instruction
    int opcode = ins.num & 0x7F;
    switch (opcode) {
        case 0x33:{           // Type R
            int funct7 = ins.num >> 25;
            int funct3 = (ins.num >> 12) & 0x7;
            int rd = (ins.num >> 7) & 0x1F;
            Word rs1 = Register.GetReg((ins.num >> 15) & 0x1F);
            Word rs2 = Register.GetReg((ins.num >> 20) & 0x1F);
            p = new TypeRIns(opcode, rs1, rs2, funct3, funct7, rd);
            break;
        }
        case 0x13:case 0x67: case 0x3:{                           // Type I
            int funct3 = (ins.u_num >> 12u) & 0x7;
            int rd = (ins.u_num >> 7u) & 0x1F;
            Word rs1 = Register.GetReg((ins.num >> 15) & 0x1F);
            Word imm11_0 = Word(ins.num >> 20);
            p = new TypeIIns(opcode, rs1, imm11_0, funct3, rd);
            break;
        }
        case 0x23:{                                     // Type S
            int funct3 = (ins.u_num >> 12) & 0x7;
            Word rs1 = Register.GetReg((ins.u_num >> 15) & 0x1F);
            Word rs2 = Register.GetReg((ins.u_num >> 20) & 0x1F);
            Word imm11_5 = Word(ins.u_num >> 25);
            Word imm4_5 = Word((ins.u_num >> 7) & 0x1F);
            Word imm = Word(((ins.u_num >> 25) << 5)  | (ins.u_num >> 7) & 0x1F);
            if (imm.u_num >> 11 == 1) {
                imm.u_num |= 0xFFFFFu << 12u;
            }
            p = new TypeSIns(opcode, rs1, rs2, imm, funct3);
            break;
        }
        case 0x63:{                                                // Type B
            int funct3 = (ins.num >> 12) & 0x7;
            Word rs1 = Register.GetReg((ins.u_num >> 15u) & 0x1Fu);
            Word rs2 = Register.GetReg((ins.u_num >> 20u) & 0x1Fu);
            Word imm12 = (ins.u_num >> 31u);
            Word imm10_5 = (ins.u_num >> 25u) & 0x3Fu;
            Word imm4_1 = (ins.u_num >> 8u) & 0xFu;
            Word imm11 = (ins.u_num >> 7u) & 0x1u;
            Word imm(0);
            imm.u_num = (imm12.u_num << 12u) | (imm10_5.u_num << 5u) | (imm11.u_num << 11u) | (imm4_1.u_num << 1u);
            if (imm12.num == 1) {
                imm.u_num |= (0xFFFFFu << 12u);
            }
            p = new TypeBIns(opcode, rs1, rs2, imm, funct3);
            break;
        }
        case 0x37: case 0x17:{                                  //  Type U
            int rd = (ins.num >> 7) & 0x1F;
            Word imm(0);
            imm.u_num = (ins.u_num >> 12u) << 12u;
            p = new TypeUIns(opcode, imm, rd);
            break;
        }
        case 0x6F:{                                              // Type J
            int rd = (ins.u_num >> 7) & 0x1F;
            Word imm20 = ins.u_num >> 31;
            Word imm10_1 = (ins.u_num >> 21) & 0x3FF;
            Word imm11 = (ins.u_num >> 20) & 0x1;
            Word imm19_12 = (ins.u_num >> 12) & 0xFF;
            Word imm(0);
            imm.u_num = (imm10_1.u_num << 1) | (imm11.u_num << 11) | (imm19_12.u_num << 12) | (imm20.u_num == 1 ? 0xFFF00000 : 0);
            p = new TypeJIns(opcode, imm, rd);
        }
    }

}

#endif //RISC_V_INSTRUCTION_HPP
