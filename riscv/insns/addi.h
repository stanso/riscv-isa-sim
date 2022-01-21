WRITE_RD(sext_xlen(RS1 + insn.i_imm()));

// Generate ld for dp_bindld
// auto bind_reg = p->get_csr(CSR_BINDMEM0 + insn.rd());
// if (bind_reg != 0)
// {
//   WRITE_REG(bind_reg, MMU.load_int64(RD));
// }
