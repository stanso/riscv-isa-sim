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
        // get the length part and write to the rd register
        WRITE_REG(insn.rd(), vbindmem_raw >> 5);
        break;
    }
}

// get corresponding vbindmemX register & the value
// int csr = validate_csr(CSR_VBINDMEM0 + insn.rs1(), true);
// reg_t old = p->get_csr(csr, insn, true);

// get the length part and write to the rd register
// WRITE_REG(insn.rd(), old >> 5);

// serialize();