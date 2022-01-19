require_rv64;
MMU.store_uint64(RS1 + insn.s_imm(), RS2);
WRITE_REG(insn.rs1(), sext_xlen(RS1 + 8));
