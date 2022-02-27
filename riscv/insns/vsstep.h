// vsstep n_reg, desc
#include "decode.h"
#include <cstdint>
require_vector(true);
VI_CHECK_SLIDE(true);

int csr = validate_csr(CSR_VBINDMEMDESC0 + insn.i_imm(), true);
reg_t vbindmemdesc_val = p->get_csr(csr, insn, true);
reg_t adv_elem_cnt = RS1;

// Iterate through all 12 slots in the descriptor
for (int i = 0; i < 12; i++)
{
    // Get the current addr reg
    reg_t cur_addr_reg_num = (vbindmemdesc_val >> (4 + i * 5)) & 31;
    // if this is not an empty slot
    if (cur_addr_reg_num != 0)
    {
        // read corresponding vbindmemX register
        csr = validate_csr(CSR_VBINDMEM0 + cur_addr_reg_num, true);
        reg_t vbindmem_raw = p->get_csr(csr, insn, true);

        // Get the val reg number & stream length left
        reg_t val_reg_num = vbindmem_raw & 31;
        reg_t stream_len_left = vbindmem_raw & ~((reg_t)31);

        // advance address
        reg_t ld_addr = READ_REG(cur_addr_reg_num) + 8 * adv_elem_cnt;
        WRITE_REG(cur_addr_reg_num, sext_xlen(ld_addr));

        // NOTE: not resecting vstart, nf, masking
        // load elements
        const reg_t baseAddr = ld_addr;
        const reg_t vd = insn.rd();
        const reg_t elt_per_reg = P.VU.vlenb / sizeof(uint64_t);
        for (reg_t i = 0; i < elt_per_reg; ++i)
        {
            auto &vd = P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i, true);
            if (i + adv_elem_cnt < elt_per_reg)
                // shift elements to the front
                vd = P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i + adv_elem_cnt);
            else
                // load new elements to the end
                vd = MMU.load_uint64(baseAddr + (i - elt_per_reg) * sizeof(uint64_t));
        }
        P.VU.vstart->write(0);
    }
}