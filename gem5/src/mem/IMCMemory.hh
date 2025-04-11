#ifndef __MEM_IMCMEMORY_HH__
#define __MEM_IMCMEMORY_HH__

#include "mem/simple_mem.hh"  // Base memory model from GEM5
#include "params/IMCMemory.hh"

class IMCMemory : public SimpleMemory
{
  public:
    IMCMemory(const IMCMemoryParams &p);
    
    // Runs when GEM5 starts
    void startup();

    void executeAddition(uint64_t addr1, uint64_t addr2, uint64_t resultAddr);
    void processRequest(PacketPtr pkt) override;
};

#endif // __MEM_IMCMEMORY_HH__