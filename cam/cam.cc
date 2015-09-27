#include "cam.h"

static reg_t cam0(processor_t* p, insn_t insn, reg_t pc)
{
  cam_t* const ext = static_cast<cam_t*>(p->get_extension());
  const reg_t xs1 = RS1;
  const reg_t xs2 = RS2;
  const int64_t xii = insn.i_imm();
  const int64_t xis = insn.s_imm();
  const uint64_t f7 = insn.funct7();

  switch (insn.rm()) {
    case 0: /* CAMSK0 */
      ext->camsk0(xs1, xs2, xis);
      //xcam::opcodes::camsk0_sw(id, xs1, xis, xs2);
      break;
    case 1: /* CAMSK1 */
      ext->camsk1(xs1, xs2, xis);
      break;
    case 2: /* CAMSV */
      ext->camsv(xs1, xs2, xis);
      break;
    case 3: /* CAMLK0 */
      WRITE_RD(ext->camlk0(xs1, xii));
      break;
    case 4: /* CAMLK1 */
      WRITE_RD(ext->camlk1(xs1, xii));
      break;
    case 5: /* CAMLV */
      WRITE_RD(ext->camlv(xs1, xii));
      break;
    case 6: /* CAM */
      WRITE_RD(ext->cam(xs1, xs2, f7));
      break;
    default:
      ext->cam_illegal_instruction();
  }

  return pc + 4;
}

std::vector<insn_desc_t> cam_t::get_instructions()
{
  std::vector<insn_desc_t> insns;
  insns.push_back((insn_desc_t){0x0b, 0x7f, cam0, cam0});
  return insns;
}

extern load_address_t load_address;
extern store_address_t store_address;
extern xrd_t xrd;
extern xrs1_t xrs1;
extern xrs2_t xrs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.funct7()) + '(' + xpr_name[insn.rs2()] + ')';
  }
} maskKey1;

std::vector<disasm_insn_t*> cam_t::get_disasms()
{
  std::vector<disasm_insn_t*> insns;
  insns.push_back(new disasm_insn_t("camsk0", 0x000b, 0x707f, {&xrs2, &store_address}));
  insns.push_back(new disasm_insn_t("camsk1", 0x100b, 0x707f, {&xrs2, &store_address}));
  insns.push_back(new disasm_insn_t("camsv",  0x200b, 0x707f, {&xrs2, &store_address}));
  insns.push_back(new disasm_insn_t("camlk0", 0x300b, 0x707f, {&xrd, &load_address}));
  insns.push_back(new disasm_insn_t("camlk1", 0x400b, 0x707f, {&xrd, &load_address}));
  insns.push_back(new disasm_insn_t("camlv",  0x500b, 0x707f, {&xrd, &load_address}));
  insns.push_back(new disasm_insn_t("cam",    0x600b, 0x707f, {&xrd, &xrs1, &maskKey1}));
  return insns;
}

REGISTER_EXTENSION(cam, []() { return new cam_t; })
