#include <iostream>
#include "memory.hpp"
#include "register.hpp"
#include "instruction.hpp"
#include <string>

int main() {
    // read to memory
    unsigned int address = 0;
    std::string input;
    while (std::cin >> input) {
        if (input[0] == '@') {      // update address
            input.erase(0,1);
            address = std::stoi(input, nullptr, 16);
        } else {
            if (input[0] == EOF) break;
            memory.mem[address] = Byte(std::stoi(input, nullptr, 16));
            address++;
        }
    }

    // run
    Register.Setpc(Word(0));
    Word ins;
    while (true) {
        AbstractIns* p;

        Word pc = Register.Getpc();

        ins = InstructionFetch(pc);

        if (ins.u_num == 0x0ff00513) {
            break;
        }

        InstructionDecode(ins, p);

        p->Execute();

        p->MemoryAccess();

        p->WriteBack();

        if (!Register.jumpFlag) {
            Register.Setpc(Register.Getpc().num + 4);
        } else {
            Register.jumpFlag = false;
            //std::cout << "pc" << pc.num << std::endl;
            //std::cout << memory.mem[0x1884] << std::endl;
        }

        delete p;
    } ;

    std::cout << (Register.GetReg(10).u_num & 255u) << std::endl;

    return 0;
}