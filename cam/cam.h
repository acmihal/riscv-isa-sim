#ifndef _RISCV_CAM_H
#define _RISCV_CAM_H

#include <iostream>
#include <vector>
#include "extension.h"

class cam_t : public extension_t {
public:
    const char* name() { return "cam"; }

    std::vector<insn_desc_t> get_instructions();
    std::vector<disasm_insn_t*> get_disasms();

    reg_t caml(reg_t xs1, int xi) {
        if (xs1 + xi >= scratchpad.size()) {
            scratchpad.resize(xs1 + xi + 1);
        }
        std::cout << "caml scratchpad[" << (xs1 + xi) << "] => " << scratchpad[xs1+xi] << std::endl;
        return scratchpad[xs1 + xi];
    }

    void cams(reg_t xs1, reg_t xs2, reg_t xi) {
        if (xs1 + xi >= scratchpad.size()) {
            scratchpad.resize(xs1 + xi + 1);
        }
        std::cout << "cams scratchpad[" << (xs1 + xi) << "] <= " << xs2 << std::endl;
        scratchpad[xs1 + xi] = xs2;
    }

    void camx() {
        illegal_instruction();
    }

    std::vector<int> scratchpad;
};

#endif /* _RISCV_CAM_H */
