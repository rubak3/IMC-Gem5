#ifndef __ARCH_RISCV_INSTS_ADDMOD_HH__
#define __ARCH_RISCV_INSTS_ADDMOD_HH__

#include "arch/riscv/insts/static_inst.hh"

namespace gem5::RiscvISA {

class AddmodInst : public RiscvStaticInst
{
  public:
    AddmodInst(ExtMachInst _machInst);

    Fault execute(ExecContext *xc, trace::InstRecord *traceData) const override;
};

} // namespace gem5::RiscvISA

#endif // __ARCH_RISCV_INSTS_ADDMOD_HH__