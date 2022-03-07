// vsbindle64.v vd, vs, imm5
#include "decode.h"

// require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);

reg_t rd_num = insn.rd();
reg_t rs2_num = insn.rs2();

// 1. Load the vs to the vd
// REMOVED

// 2. Store val register number into the vbindmemX[0:4]
int csr = CSR_VBINDMEM0 + rs2_num;
reg_t old = p->get_csr(csr, insn, true);
p->set_csr(csr, (old & ~((uint64_t)31)) | (rd_num & 31));
// serialize();

// fprintf(stderr, "SPIKE: rs2_num = %lu\n", rs2_num);
// 3. [ignored, not necessary in CAM HW] Store the # of binds to the corresponding vbindmemdescX[0:3]?
// 4. Store the addr reg # to on of the slot in vbindmemdescX
// csr = validate_csr(CSR_VBINDMEMDESC0 + insn.v_zimm5(), true);
csr = CSR_VBINDMEMDESC0 + insn.v_zimm5();
old = p->get_csr(csr, insn, true);

old = old | 1; // set the first bit to indicate store stream
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
    p->set_csr(csr,  (rs2_num << (4 + free_pos * 5)) | old);
else
    p->set_csr(csr,  old);
// serialize();
// fprintf(stderr, "SPIKE: set csr = %lu\n", p->get_csr(csr, insn, true));
