require_rv64;
WRITE_REG(insn.rs1(), sext_xlen(RS1 + 4));
WRITE_REG(insn.rs2(), sext_xlen(RS2 + 4));
WRITE_REG(insn.rd(), sext_xlen(RD + 4));


// Generate ld for dp_bindld
auto bind_reg = p->get_csr(CSR_BINDMEM0 + insn.rs1());
if (bind_reg != 0)
{
  WRITE_REG(bind_reg, MMU.load_int32(RS1));
}

bind_reg = p->get_csr(CSR_BINDMEM0 + insn.rs2());
if (bind_reg != 0)
{
  WRITE_REG(bind_reg, MMU.load_int32(RS2));
}

bind_reg = p->get_csr(CSR_BINDMEM0 + insn.rd());
if (bind_reg != 0)
{
  WRITE_REG(bind_reg, MMU.load_int32(RD));
}

