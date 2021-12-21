require_rv64;
WRITE_RD(MMU.load_int64(RS1 + insn.i_imm()));
WRITE_REG(insn.rs1(), sext_xlen(RS1 + 4));
