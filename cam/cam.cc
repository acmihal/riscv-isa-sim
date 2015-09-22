#include "cam.h"

static reg_t cam0(processor_t* p, insn_t insn, reg_t pc)
{
    cam_t* ext = static_cast<cam_t*>(p->get_extension());
    switch(insn.rm()) {
        case 0: { /* CAML */
            reg_t xs1 = RS1;
            int xi = insn.i_imm();
            reg_t xd = ext->caml(xs1, xi);
            WRITE_RD(xd);
            break;
        }
        default: {
            ext->camx();
        }
    }
    return pc+4;
}

static reg_t cam1(processor_t* p, insn_t insn, reg_t pc)
{
    cam_t* ext = static_cast<cam_t*>(p->get_extension());
    switch(insn.rm()) {
        case 0: { /* CAMS */
            reg_t xs1 = RS1;
            reg_t xs2 = RS2;
            int xi = insn.s_imm();
            ext->cams(xs1, xs2, xi);
            break;
        }
        default: {
            ext->camx();
        }
    }
    return pc+4;
}

std::vector<insn_desc_t> cam_t::get_instructions()
{
  std::vector<insn_desc_t> insns;
  insns.push_back((insn_desc_t){0x0b, 0x7f, cam0, cam0});
  insns.push_back((insn_desc_t){0x2b, 0x7f, cam1, cam1});
  return insns;
}

extern load_address_t load_address;
extern store_address_t store_address;
extern xrd_t xrd;
extern xrs1_t xrs1;
extern xrs2_t xrs2;

std::vector<disasm_insn_t*> cam_t::get_disasms()
{
  std::vector<disasm_insn_t*> insns;
  insns.push_back(new disasm_insn_t("caml", 0x0b, 0x707f, {&xrd, &load_address}));
  insns.push_back(new disasm_insn_t("cams", 0x2b, 0x707f, {&xrs2, &store_address}));
  return insns;
}

REGISTER_EXTENSION(cam, []() { return new cam_t; })
