require_rv64;
// RS1 => source addr for ld/sd
// RS2 => loaded value
int csr = validate_csr(CSR_BINDMEM0 + insn.rs1(), true);
reg_t old = p->get_csr(csr, insn, true);
p->set_csr(csr, insn.rs2());
WRITE_RD(sext_xlen(old));
WRITE_REG(insn.rs2(), MMU.load_int64(RS1));
serialize();
