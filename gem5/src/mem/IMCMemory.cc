#include "mem/IMCMemory.hh"
#include "debug/IMCMemory.hh"

IMCMemory::IMCMemory(const IMCMemoryParams &p) : SimpleMemory(p) {}

// Automatically runs when GEM5 starts
void IMCMemory::startup()
{
    uint64_t testAddr1 = 0x100000;  // Arbitrary memory address for testing
    uint64_t testAddr2 = 0x100008;
    uint64_t resultAddr = 0x100010;

    // Assign test values directly in memory
    writeMem(testAddr1, 15);
    writeMem(testAddr2, 25);

    // Execute the addition inside memory
    executeAddition(testAddr1, testAddr2, resultAddr);

    // Read the result and print it
    uint64_t result = readMem(resultAddr);
    printf("[IMC] Startup Addition Test: 15 + 25 = %lu\n", result);
}

// Perform in-memory addition
void IMCMemory::executeAddition(uint64_t addr1, uint64_t addr2, uint64_t resultAddr)
{
    uint64_t val1 = readMem(addr1);
    uint64_t val2 = readMem(addr2);

    uint64_t sum = val1 + val2;

    writeMem(resultAddr, sum);
}

// Handle memory requests
void IMCMemory::processRequest(PacketPtr pkt)
{
    if (pkt->isIMCAdditionRequest()) {  // Check if request is an IMC ADD request
        uint64_t addr1 = pkt->getAddr1();
        uint64_t addr2 = pkt->getAddr2();
        uint64_t resultAddr = pkt->getResultAddr();

        executeAddition(addr1, addr2, resultAddr);  // Perform in-memory addition
        pkt->makeResponse();  // Mark request as completed
    } else {
        SimpleMemory::processRequest(pkt);  // Default memory behavior
    }
}