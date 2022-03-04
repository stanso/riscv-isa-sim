// vsbindle64.v vd, vs, imm5
#include "decode.h"
#include <cstdint>

// require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);

reg_t rd_num = insn.rd();
reg_t rs2_num = insn.rs2();

// reg_t vl = P.VU.vl->read();
// reg_t sew = P.VU.vsew;

// 1. Load the vs to the vd
// VI_LD(0, (i * nf + fn), int64, false);

// const reg_t nf = insn.v_nf() + 1;
// const reg_t baseAddr = RS2;
// const reg_t vd = insn.rd();

// fprintf(stderr, "SPIKE: rd_num = %lu, rs2_num = %lu, rs2_val = %x\n", rd_num, rs2_num, baseAddr);

// VI_CHECK_LOAD(int64, false);
// for (reg_t i = 0; i < vl; ++i)
// {
//     VI_ELEMENT_SKIP(i);
//     VI_STRIP(i);
//     P.VU.vstart->write(i);
//     for (reg_t fn = 0; fn < nf; ++fn) {
//         int64_t val = MMU.load_uint64(baseAddr + (0) + (i * nf + fn) * sizeof(int64_t));
//         P.VU.elt<int64_t>(vd + fn * emul, vreg_inx, true) = val;
//     }
// }

// const reg_t baseAddr = READ_REG(rs2_num);
// const reg_t vd = rd_num;
// const reg_t elt_per_reg = P.VU.vlenb / sizeof(uint64_t);
// for (reg_t i = 0; i < elt_per_reg; ++i)
// {
//     auto &vd = P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i, true);
//     if (i >= stream_len_left)
//         vd = UINT64_MAX;
//     else if (i + adv_elem_cnt < elt_per_reg)
//         // shift elements to the front
//         vd = P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i + adv_elem_cnt);
//     else
//         // load new elements to the end
//         vd = MMU.load_uint64(baseAddr + (i - elt_per_reg) * sizeof(uint64_t));
// }

// P.VU.vstart->write(0);

// 2. Store val register number into the vbindmemX[0:4]
int csr = CSR_VBINDMEM0 + rs2_num;
reg_t old = p->get_csr(csr, insn, true);
p->set_csr(csr, (old & ~((uint64_t)31)) | (rd_num & 31));

// fprintf(stderr, "SPIKE: rs2_num = %lu\n", rs2_num);
// 3. [ignored, not necessary in CAM HW] Store the # of binds to the corresponding vbindmemdescX[0:3]?
// 4. Store the addr reg # to on of the slot in vbindmemdescX
csr = validate_csr(CSR_VBINDMEMDESC0 + insn.v_zimm5(), true);
// fprintf(stderr, "SPIKE: v-zimm5 = %lu\n", insn.v_zimm5());
old = p->get_csr(csr, insn, true);
int free_pos = -1;
bool exist = false;
for (int i = 0; i < 12; i++)
{
    reg_t tmp = (old >> (4 + i * 5)) & 31;
    if (rs2_num == tmp)
    {
        exist = true;
        break;
    }
    if (free_pos == -1 && tmp == 0)
    {
        free_pos = i;
    }
}
if (!exist)
{
    p->set_csr(csr,  (rs2_num << (4 + free_pos * 5)) | old);
}
// fprintf(stderr, "SPIKE: set csr = %lu\n", p->get_csr(csr, insn, true));
