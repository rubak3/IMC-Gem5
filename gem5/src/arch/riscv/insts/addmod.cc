#include "arch/riscv/insts/static_inst.hh"
#include "cpu/exec_context.hh"
#include "arch/riscv/insts/addmod.hh"


using namespace gem5;

namespace gem5::RiscvISA {

class AddmodInst : public RiscvStaticInst {
public:
    AddmodInst(ExtMachInst _machInst)
        : RiscvStaticInst("ADDMOD", _machInst, No_OpClass) {}

    Fault execute(ExecContext *xc, trace::InstRecord *traceData) const override {
        auto rs1 = xc->readIntReg(bits(machInst, 19, 15));
        auto rs2 = xc->readIntReg(bits(machInst, 24, 20));
        auto rs3 = xc->readIntReg(bits(machInst, 31, 27));
        auto rd  = bits(machInst, 11, 7);

        auto result = (rs1 + rs2) % rs3;
        xc->setIntReg(rd, result);

        DPRINTF(IMC, "ADDmodInst: (%u + %u) %% %u = %u → x%d\n", rs1, rs2, rs3, result, rd);
        return NoFault;
    }
};

} // namespace gem5::RiscvISA