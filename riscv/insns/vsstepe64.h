// vsstep n_reg, desc
#include "decode.h"
#include <cstdint>
require_vector(true);

// int csr = validate_csr(CSR_VBINDMEMDESC0 + insn.i_imm(), true);
int csr = CSR_VBINDMEMDESC0 + insn.i_imm();
reg_t vbindmemdesc_val = p->get_csr(csr, insn, true);
reg_t adv_elem_cnt = RS1;
// serialize();

bool is_store = vbindmemdesc_val & 1;

// Iterate through all 12 slots in the descriptor
for (int i = 0; i < 12; i++)
{
    if (i < STATE.internal_mem_state)
        continue;

    // Get the current addr reg
    reg_t cur_addr_reg_num = (vbindmemdesc_val >> (4 + i * 5)) & 31;
    // if this is not an empty slot
    if (cur_addr_reg_num != 0)
    {
        // clear RD which contains number of elements streamed from memory
        WRITE_RD(0);

        // read corresponding vbindmemX register
        // csr = validate_csr(CSR_VBINDMEM0 + cur_addr_reg_num, true);
        csr = CSR_VBINDMEM0 + cur_addr_reg_num;
        reg_t vbindmem_raw = p->get_csr(csr, insn, true);

        // Get the val reg number & stream length left
        reg_t val_reg_num = vbindmem_raw & 31;
        reg_t stream_len_left = (vbindmem_raw & ~((reg_t)31)) >> 5;

        // fprintf(stderr, "SPIKE: vsstep, stream_len_left = %lu\n", stream_len_left);

        const reg_t baseAddr = READ_REG(cur_addr_reg_num);
        // advance address
        // WRITE_REG(cur_addr_reg_num, sext_xlen(baseAddr + 8 * (adv_elem_cnt < stream_len_left ? adv_elem_cnt : stream_len_left)));

        // fprintf(stderr, "SPIKE: vsstepe64, base = %p, basep = %p\n", baseAddr, baseAddr + 8 * adv_elem_cnt);

        if (!is_store)
        {
            // NOTE: not respecting vstart, nf, masking
            // load elements
            const reg_t elt_per_reg = P.VU.vlenb / sizeof(uint64_t);
            for (reg_t i = 0; i < elt_per_reg; ++i)
            {
                if (i < P.VU.vstart->read())
                    continue;
                P.VU.vstart->write(i);
                auto &vd = P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i, true);
                if (i + adv_elem_cnt < elt_per_reg)
                    // shift elements to the front
                    vd = P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i + adv_elem_cnt);
                else if (i + adv_elem_cnt - elt_per_reg < stream_len_left)
                {
                    // load new elements to the end
                    vd = MMU.load_uint64(baseAddr + (i + adv_elem_cnt - elt_per_reg) * sizeof(uint64_t));
                    // advance address
                    WRITE_REG(cur_addr_reg_num, sext_xlen(baseAddr + 8));
                    stream_len_left --;
                    p->set_csr(csr, (stream_len_left << 5) | (vbindmem_raw & 31)); // update stream length, need preserve the [0:4] bits from raw
                    WRITE_RD(RD + 1);
                }
                else
                    vd = UINT64_MAX;
            }
            P.VU.vstart->write(0);
            // if (stream_len_left < adv_elem_cnt)
            //     stream_len_left = 0;
            // else
            //     stream_len_left -= adv_elem_cnt;
        }
        else
        {
            const reg_t elt_per_reg = P.VU.vlenb / sizeof(uint64_t);
            for (reg_t i = 0; i < elt_per_reg; ++i)
            {
                if (i < P.VU.vstart->read())
                    continue;
                P.VU.vstart->write(i);
                auto &vd = P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i, true);
                if (i < adv_elem_cnt && i + adv_elem_cnt < elt_per_reg)
                {
                    // shift elements to the front
                    MMU.store_uint64(baseAddr + i * sizeof(uint64_t), P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i));
                    vd = P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i + adv_elem_cnt);
                    WRITE_REG(cur_addr_reg_num, sext_xlen(baseAddr + 8));
                    stream_len_left ++;
                    p->set_csr(csr, (stream_len_left << 5) | (vbindmem_raw & 31)); // update stream length, need preserve the [0:4] bits from raw
                    WRITE_RD(RD + 1);
                }
                else if (i < adv_elem_cnt)
                {
                    MMU.store_uint64(baseAddr + i * sizeof(uint64_t), P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i));
                    vd = UINT64_MAX;
                    WRITE_REG(cur_addr_reg_num, sext_xlen(baseAddr + 8));
                    stream_len_left ++;
                    p->set_csr(csr, (stream_len_left << 5) | (vbindmem_raw & 31)); // update stream length, need preserve the [0:4] bits from raw
                    WRITE_RD(RD + 1);
                }
                else if (i + adv_elem_cnt < elt_per_reg)
                    vd = P.VU.elt<type_sew_t<e64>::type>(val_reg_num, i + adv_elem_cnt);
                else
                    vd = UINT64_MAX;
            }
            P.VU.vstart->write(0);
        }

        // p->set_csr(csr, (stream_len_left << 5) | (vbindmem_raw & 31)); // update stream length, need preserve the [0:4] bits from raw
        // serialize();
    }
    STATE.internal_mem_state = i;
}

STATE.internal_mem_state = 0;