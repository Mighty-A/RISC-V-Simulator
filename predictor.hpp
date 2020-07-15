//
// Created by yhy79 on 2020/7/13.
//

#ifndef RISC_V_PREDICTOR_HPP
#define RISC_V_PREDICTOR_HPP
class Predictor {
public:
    int status;
    int count;
    int countCorrect;
    Predictor() : status(0), count(0), countCorrect(0){
    }

    bool IfBranch() {
        count++;
        return status & 2;
    }

    void Update(bool ifBranch) {
        if (ifBranch) {
            if (status < 2) {
                status++;
            } else if (status < 3) {
                status++;
                countCorrect++;
            } else {
                countCorrect++;
            }
        } else {
            if (status > 1) {
                status--;
            } else if (status > 0) {
                status--;
                countCorrect++;
            } else {
                countCorrect++;
            }
        }
    }
} predictor[32];
#endif //RISC_V_PREDICTOR_HPP
