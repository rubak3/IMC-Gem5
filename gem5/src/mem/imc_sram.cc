#include "mem/imc_sram.hh"
#include "mem/packet.hh"
#include "debug/IMC.hh"

namespace gem5
{

IMCSRAM::IMCSRAM(const Params &params)
    : SimpleMemory(params)
{}

Tick
IMCSRAM::recvAtomic(PacketPtr pkt)
{
    if (pkt->req->hasFlags(Request::IS_ADDMOD)) {
        DPRINTF(IMC, "IMCSRAM received addmod IMC packet\n");
        uint64_t operand = pkt->get<uint64_t>();
        uint64_t result = (0 + operand) % 6;
        *((uint64_t*)pkt->getPtr<uint64_t>()) = result;
        pkt->makeResponse();
        return latency;
    }

    return SimpleMemory::recvAtomic(pkt);
}

// ✅ Register this SimObject cleanly with gem5
} // namespace gem5

GEM5_REGISTER_CLASS(gem5::IMCSRAM);