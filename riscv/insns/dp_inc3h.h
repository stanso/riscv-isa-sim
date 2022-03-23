require_rv64;

if (STATE.internal_mem_state == 0)
{
  WRITE_REG(insn.rs1(), sext_xlen(RS1 + 2));
  WRITE_REG(insn.rs2(), sext_xlen(RS2 + 2));
  WRITE_REG(insn.rd(), sext_xlen(RD + 2));
  STATE.internal_mem_state = 1;
}

// Generate ld for dp_bindld
auto bind_reg = p->get_csr(CSR_BINDMEM0 + insn.rs1());

if (STATE.internal_mem_state == 1)
{
  if (bind_reg != 0)
  {
    WRITE_REG(bind_reg, MMU.load_int16(RS1));
  }
  STATE.internal_mem_state = 2;
}

if (STATE.internal_mem_state == 2)
{
  bind_reg = p->get_csr(CSR_BINDMEM0 + insn.rs2());
  if (bind_reg != 0)
  {
    WRITE_REG(bind_reg, MMU.load_int16(RS2));
  }
  STATE.internal_mem_state = 3;
}

if (STATE.internal_mem_state == 3)
{
  bind_reg = p->get_csr(CSR_BINDMEM0 + insn.rd());
  if (bind_reg != 0)
  {
    WRITE_REG(bind_reg, MMU.load_int16(RD));
  }
  STATE.internal_mem_state = 0;
}
