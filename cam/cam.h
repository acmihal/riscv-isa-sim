#ifndef _RISCV_CAM_H
#define _RISCV_CAM_H

#include "extension.h"

class cam_t : public extension_t {
public:

  const char* name() { return "cam"; }

  std::vector<insn_desc_t> get_instructions();
  std::vector<disasm_insn_t*> get_disasms();

  void cam_illegal_instruction() {
    illegal_instruction();
  }

};

#endif /* _RISCV_CAM_H */
