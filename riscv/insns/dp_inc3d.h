require_rv64;
WRITE_REG(insn.rs1(), sext_xlen(RS1 + 1));
WRITE_REG(insn.rs2(), sext_xlen(RS2 + 1));
WRITE_REG(insn.rd(), sext_xlen(RD + 1));
