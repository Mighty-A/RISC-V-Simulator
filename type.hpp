//
// Created by yhy79 on 2020/7/6.
//

#ifndef RISC_V_TYPE_HPP
#define RISC_V_TYPE_HPP
class Byte {                            // type Byte
public:
    unsigned char b;
    Byte(unsigned char b) : b(b) {}
    Byte() {b = 0;}
    operator int() {
        return int(b);
    }
};

/*
 * use union to deal with the space that may be both one int and four char
 */
union HalfWord {                        // type HalfWord
    short num;
    unsigned short u_num;
    struct {
        unsigned char b1, b2;
    } b;
public:
    HalfWord() {num = 0;}
    HalfWord(short num) : num(num) {}
    HalfWord(unsigned u_num) : u_num(u_num) {}
    HalfWord(unsigned char b1, unsigned char b2) {
        b.b1 = b1;
        b.b2 = b2;
    }
    operator int() {
        return int(num);
    }
};

union Word {                            // type Word
    int num;
    unsigned int u_num;
    struct {
        unsigned char b4, b3, b2, b1;
    } b;
public:
    Word() {num = 0;}
    Word(int num) : num(num) {}
    Word(unsigned int u_num) : u_num(u_num) {}
    Word(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4) {
        b.b1 = b1;
        b.b2 = b2;
        b.b3 = b3;
        b.b4 = b4;
    }
    operator int() {
        return num;
    }
};
#endif //RISC_V_TYPE_HPP
