// vssetlen len_reg, desc
require_vector(true);

// int csr = validate_csr(CSR_VBINDMEMDESC0 + insn.i_imm(), true);
int csr = CSR_VBINDMEMDESC0 + insn.i_imm();
reg_t vbindmemdesc_val = p->get_csr(csr, insn, true);
bool is_store = vbindmemdesc_val & 1;

// Iterate through all 12 slots in the descriptor
for (int i = 0; i < 12; i++)
{
    // Get the current addr reg
    reg_t cur_addr_reg_num = (vbindmemdesc_val >> (4 + i * 5)) & 31;
    // if this is not an empty slot
    if (cur_addr_reg_num != 0)
    {
        // read corresponding vbindmemX register
        // csr = validate_csr(CSR_VBINDMEM0 + cur_addr_reg_num, true);
        csr = CSR_VBINDMEM0 + cur_addr_reg_num;
        reg_t vbindmem_raw = p->get_csr(csr, insn, true);
        reg_t val_reg_num = vbindmem_raw & 31;
        // read the length from the length register & store into the vbindmemX register
        p->set_csr(csr, (vbindmem_raw & 31) | (READ_REG(insn.rs1()) << 5));

        // set all elements of the vector register to end markers
        for (reg_t j = 0; j < P.VU.vlmax; ++j)
        {
            switch (P.VU.vsew)
            {
            case e8:
                P.VU.elt<type_sew_t<e8>::type>(val_reg_num, j, true) = UINT8_MAX;
                break;
            case e16:
                P.VU.elt<type_sew_t<e16>::type>(val_reg_num, j, true) = UINT16_MAX;
                break;
            case e32:
                P.VU.elt<type_sew_t<e32>::type>(val_reg_num, j, true) = UINT32_MAX;
                break;
            default:
                P.VU.elt<type_sew_t<e64>::type>(val_reg_num, j, true) = UINT64_MAX;
                break;
            }
        }
    }
}


// get corresponding vbindmemX register & the value
// int csr = validate_csr(CSR_VBINDMEM0 + insn.rs1(), true);
// reg_t old = p->get_csr(csr, insn, true);

// read the length from the length register & store into the vbindmemX register
// p->set_csr(csr, (old & 31) | READ_REG(insn.rs2()) << 5);
// serialize();

// fprintf(stderr, "SPIKE: vssetlen, final csr = %lu\n", p->get_csr(csr, insn, true));