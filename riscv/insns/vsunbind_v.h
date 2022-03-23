require_vector(true);

// Clear out the vsbindmemX register (both val reg & length)
int csr = CSR_VBINDMEM0 + insn.rs2();
reg_t old = p->get_csr(csr, insn, true);
p->set_csr(csr, 0);

// int csr = validate_csr(CSR_VBINDMEMDESC0 + insn.i_imm(), true);
csr = CSR_VBINDMEMDESC0 + insn.v_zimm5();
reg_t vbindmemdesc_val = p->get_csr(csr, insn, true);
bool is_store = vbindmemdesc_val & 1;

// Iterate through all 12 slots in the descriptor
for (int i = 0; i < 12; i++)
{
    // Get the current addr reg
    reg_t cur_addr_reg_num = (vbindmemdesc_val >> (4 + i * 5)) & 31;
    // if this is not an empty slot
    if (cur_addr_reg_num == insn.rs2())
    {
        p->set_csr(csr, vbindmemdesc_val & ~(((uint64_t)31) << (4 + i * 5)));
        break;
    }
}