// vmmerge_pos.vv vd, vs2, vs1
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);
reg_t vl = P.VU.vl->read();
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();

int csr = validate_csr(CSR_VMMERGEPOSCOLLI, true);

reg_t cur_vs1_idx = 0, cur_vs2_idx = 0;
for (reg_t i = P.VU.vstart->read(); i < vl; ++i)
{
  VI_LOOP_ELEMENT_SKIP();
  uint64_t mmask = UINT64_C(1) << mpos; \
  uint64_t &vdi = P.VU.elt<uint64_t>(insn.rd(), midx, true); \
  uint64_t res = 0;

  uint64_t vs1, vs2;

  switch (sew)
  {
  case e8:
    vs1 = static_cast<uint64_t>(P.VU.elt<uint8_t>(rs1_num, cur_vs1_idx));
    vs2 = static_cast<uint64_t>(P.VU.elt<uint8_t>(rs1_num, cur_vs2_idx));
    break;
  case e16:
    vs1 = static_cast<uint64_t>(P.VU.elt<uint16_t>(rs1_num, cur_vs1_idx));
    vs2 = static_cast<uint64_t>(P.VU.elt<uint16_t>(rs1_num, cur_vs2_idx));
    break;
  case e32:
    vs1 = static_cast<uint64_t>(P.VU.elt<uint32_t>(rs1_num, cur_vs1_idx));
    vs2 = static_cast<uint64_t>(P.VU.elt<uint32_t>(rs1_num, cur_vs2_idx));
    break;
  default:
    vs1 = P.VU.elt<uint64_t>(rs1_num, cur_vs1_idx);
    vs2 = P.VU.elt<uint64_t>(rs1_num, cur_vs2_idx);
    break;
  }

  // do merge, set flag on conflict
  if (vs1 < vs2)
  {
    res = 0;
    cur_vs1_idx++;
    p->set_csr(csr, 0);
  }
  else if (vs1 > vs2)
  {
    res = 1;
    cur_vs2_idx++;
    p->set_csr(csr, 0);
  }
  else
  {
    res = 1;
    cur_vs1_idx++;
    cur_vs2_idx++;
    p->set_csr(csr, 1);
  }

  vdi = (vdi & ~mmask) | (((res) << mpos) & mmask); \
}
P.VU.vstart->write(0);
