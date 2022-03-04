// vssetlen len_reg, desc
require_vector(true);

// get corresponding vbindmemX register & the value
int csr = validate_csr(CSR_VBINDMEM0 + insn.i_imm(), true);
reg_t old = p->get_csr(csr, insn, true);

// read the length from the length register & store into the vbindmemX register
p->set_csr(csr, (old & 31) | READ_REG(insn.rs1()) << 5);
serialize();

fprintf(stderr, "SPIKE: vssetlen, final csr = %lu", p->get_csr(csr, insn, true));