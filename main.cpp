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
    while (Register.pc.num >= 0 || id_exe.p || exe_mem.p || mem_wb.p) {
        if (WriteBack()) {                          // continue means stall here
            continue;
        }
        if (MemoryAccess()) {
            continue;
        }
        if (Execute()) {
            continue;
        }
        if (InstructionDecode())  {
            continue;
        }
        if (InstructionFetch()) {
            continue;
        }
        if (if_id.ins.num == 0x0ff00513) {
            if_id.ins.num = 0;
            Register.Setpc(Word(-1));
        }
    };

    std::cout << (Register.GetReg(10).u_num & 255u) << std::endl;

    return 0;
}