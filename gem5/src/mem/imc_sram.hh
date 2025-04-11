#ifndef __MEM_IMC_SRAM_HH__
#define __MEM_IMC_SRAM_HH__

#include "mem/simple_mem.hh"

namespace gem5
{

class IMCSRAM : public SimpleMemory
{
  public:
    IMCSRAM(const Params &params);
    Tick recvAtomic(PacketPtr pkt) override;
};

} // namespace gem5

#endif // __MEM_IMC_SRAM_HH__