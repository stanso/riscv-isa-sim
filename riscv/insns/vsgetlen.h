// vssetlen len_reg, desc
require_vector(true);

// get corresponding vbindmemX register & the value
int csr = validate_csr(CSR_VBINDMEM0 + insn.rs1(), true);
reg_t old = p->get_csr(csr, insn, true);

// get the length part and write to the rd register
WRITE_REG(insn.rd(), old >> 5);

serialize();