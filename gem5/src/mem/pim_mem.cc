/*
 * Copyright (c) 2010-2013, 2015 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2001-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 #include "mem/pim_mem.hh"

 #include "base/random.hh"
 #include "base/trace.hh"
 #include "debug/Drain.hh"
 
 namespace gem5
 {
 
 namespace memory
 {
    PIMCtrl *PIMCtrl::globalPIMCtrl = nullptr;
 
    PIMCtrl::PIMCtrl(const PIMCtrlParams &p) :
     AbstractMemory(p),
     port(name() + ".port", *this),
     latency(p.latency),
     latency_var(p.latency_var),
     bandwidth(p.bandwidth),
     isBusy(false),
     retryReq(false),
     retryResp(false),
     releaseEvent([this]{ release(); }, name()),
     dequeueEvent([this]{ dequeue(); }, name()),
     backingStore(getAddrRange().size())
 {
    PIMCtrl::globalPIMCtrl = this;
 }
 
 
 void
 PIMCtrl::init()
 {
     AbstractMemory::init();
 
     backingStore.resize(range.size());
 
     if (port.isConnected()) {
         port.sendRangeChange();
     }
 }
 
 Tick
 PIMCtrl::recvAtomic(PacketPtr pkt)
 {
     panic_if(pkt->cacheResponding(), "Should not see packets where cache "
              "is responding");
 
     access(pkt);
     return getLatency();
 }
 
 Tick
 PIMCtrl::recvAtomicBackdoor(PacketPtr pkt, MemBackdoorPtr &_backdoor)
 {
     Tick latency = recvAtomic(pkt);
     getBackdoor(_backdoor);
     return latency;
 }
 
 void
 PIMCtrl::recvFunctional(PacketPtr pkt)
 {
     pkt->pushLabel(name());

     functionalAccess(pkt);
 
     bool done = false;
     auto p = packetQueue.begin();
     // potentially update the packets in our packet queue as well
     while (!done && p != packetQueue.end()) {
        done = pkt->trySatisfyFunctional(p->pkt);
        ++p;
     }

     pkt->popLabel();
 }
 
 void
 PIMCtrl::recvMemBackdoorReq(const MemBackdoorReq &req,
         MemBackdoorPtr &_backdoor)
 {
     getBackdoor(_backdoor);
 }
 
 bool
 PIMCtrl::recvTimingReq(PacketPtr pkt)
 {
     panic_if(pkt->cacheResponding(), "Should not see packets where cache "
              "is responding");
 
     panic_if(!(pkt->isRead() || pkt->isWrite()),
              "Should only see read and writes at memory controller, "
              "saw %s to %#llx\n", pkt->cmdString(), pkt->getAddr());
 
     // we should not get a new request after committing to retry the
     // current one, but unfortunately the CPU violates this rule, so
     // simply ignore it for now
     if (retryReq)
         return false;
 
     // if we are busy with a read or write, remember that we have to
     // retry
     if (isBusy) {
         retryReq = true;
         return false;
     }
 
     // technically the packet only reaches us after the header delay,
     // and since this is a memory controller we also need to
     // deserialise the payload before performing any write operation
     Tick receive_delay = pkt->headerDelay + pkt->payloadDelay;
     pkt->headerDelay = pkt->payloadDelay = 0;
 
     // update the release time according to the bandwidth limit, and
     // do so with respect to the time it takes to finish this request
     // rather than long term as it is the short term data rate that is
     // limited for any real memory
 
     // calculate an appropriate tick to release to not exceed
     // the bandwidth limit
     Tick duration = pkt->getSize() * bandwidth;
 
     // only consider ourselves busy if there is any need to wait
     // to avoid extra events being scheduled for (infinitely) fast
     // memories
     if (duration != 0) {
         schedule(releaseEvent, curTick() + duration);
         isBusy = true;
     }
 
     //IMC
     if (pkt->req->getFlags() & Request::ADDMOD) {
         std::cout << "[IMC] Performing in-memory compute for Addmod" << std::endl;
 
         //Addr addr_x = 0x1b7c0;
         //uint64_t x_value = 1;
         //memcpy(pmemAddr + (addr_x - this->getAddrRange().start()), &x_value, sizeof(uint64_t));
         //std::cout << "[IMC] Wrote x = " << x_value
           //<< " to physical address 0x" << std::hex << addr_x << std::endl;
 
 
         const uint8_t* data = pkt->getConstPtr<uint8_t>();
 
         Addr addr_a, addr_b, addr_c, addr_d;
         memcpy(&addr_a, data, sizeof(uint64_t));
         memcpy(&addr_b, data + 8, sizeof(uint64_t));
         memcpy(&addr_c, data + 16, sizeof(uint64_t));
         memcpy(&addr_d, data + 24, sizeof(uint64_t));
 
         uint8_t* hostAddr = pmemAddr;  // or whatever your backing pointer is
         Addr baseAddr = this->getAddrRange().start(); // base of memory range
 
         //std::cout << "[IMC] hostAddr = " << hostAddr << std::endl;
 
         //std::cout << "[IMC] addr_a = " << addr_a << ", addr_b = " << addr_b << ", addr_c = " << addr_c << std::endl;
 
         uint64_t val_a, val_b, val_c;
         memcpy(&val_a, hostAddr + (addr_a - baseAddr), sizeof(uint64_t));
         memcpy(&val_b, hostAddr + (addr_b - baseAddr), sizeof(uint64_t));
         memcpy(&val_c, hostAddr + (addr_c - baseAddr), sizeof(uint64_t));
 
         //std::cout << "[IMC] addr_a = 0x" << std::hex << addr_a << ", offset = 0x" << (addr_a - baseAddr) << std::endl;
         
         //std::cout << "*(hostAddr + offset) = 0x" << *(uint64_t*)(hostAddr + (addr_a - baseAddr)) << std::endl;
 
 
         //std::cout << "[IMC] a = " << val_a << ", b = " << val_b << ", c = " << val_c << std::endl;
 
         uint64_t result = (val_a + val_b) % val_c;
         memcpy(hostAddr + (addr_d - baseAddr), &result, sizeof(uint64_t));
 
         //std::cout << "[Addmod IMC] a = " << val_a << ", b = " << val_b << ", c = " << val_c
           //        << ", result = " << result << std::endl;
 
         //if (pkt->isRead()) {
             //memcpy(pkt->getPtr<uint8_t>(), &result, sizeof(result));
         //}
 
         return true;
     }
     else if (pkt->req->getFlags() & Request::SUBMOD) {
         std::cout << "[IMC] Performing in-memory compute for Submod" << std::endl;
 
         //Addr addr_x = 0x1b7c0;
         //uint64_t x_value = 1;
         //memcpy(pmemAddr + (addr_x - this->getAddrRange().start()), &x_value, sizeof(uint64_t));
         //std::cout << "[IMC] Wrote x = " << x_value
           //<< " to physical address 0x" << std::hex << addr_x << std::endl;
 
 
         const uint8_t* data = pkt->getConstPtr<uint8_t>();
 
         Addr addr_a, addr_b, addr_c, addr_d;
         memcpy(&addr_a, data, sizeof(uint64_t));
         memcpy(&addr_b, data + 8, sizeof(uint64_t));
         memcpy(&addr_c, data + 16, sizeof(uint64_t));
         memcpy(&addr_d, data + 24, sizeof(uint64_t));
 
         uint8_t* hostAddr = pmemAddr;  // or whatever your backing pointer is
         Addr baseAddr = this->getAddrRange().start(); // base of memory range
 
         //std::cout << "[IMC] hostAddr = " << hostAddr << std::endl;
 
         //std::cout << "[IMC] addr_a = " << addr_a << ", addr_b = " << addr_b << ", addr_c = " << addr_c << std::endl;
 
         int64_t val_a, val_b, val_c;
         memcpy(&val_a, hostAddr + (addr_a - baseAddr), sizeof(int64_t));
         memcpy(&val_b, hostAddr + (addr_b - baseAddr), sizeof(int64_t));
         memcpy(&val_c, hostAddr + (addr_c - baseAddr), sizeof(int64_t));
 
         //std::cout << "[IMC] addr_a = 0x" << std::hex << addr_a << ", offset = 0x" << (addr_a - baseAddr) << std::endl;
         
         //std::cout << "*(hostAddr + offset) = 0x" << *(uint64_t*)(hostAddr + (addr_a - baseAddr)) << std::endl;
 
 
         //std::cout << "[IMC] a = " << val_a << ", b = " << val_b << ", c = " << val_c << std::endl;
 
         int64_t sub = val_a - val_b;
         int64_t result = ((sub % val_c) + val_c) % val_c;
         memcpy(hostAddr + (addr_d - baseAddr), &result, sizeof(int64_t));
 
         //std::cout << "[Addmod IMC] a = " << val_a << ", b = " << val_b << ", c = " << val_c
           //        << ", result = " << result << std::endl;
 
         //if (pkt->isRead()) {
             //memcpy(pkt->getPtr<uint8_t>(), &result, sizeof(result));
         //}
 
         return true;
     }
 
     else if (pkt->req->getFlags() & Request::MULMOD) {
         std::cout << "[IMC] Performing in-memory compute for Mulmod" << std::endl;
 
         //Addr addr_x = 0x1b7c0;
         //uint64_t x_value = 1;
         //memcpy(pmemAddr + (addr_x - this->getAddrRange().start()), &x_value, sizeof(uint64_t));
         //std::cout << "[IMC] Wrote x = " << x_value
           //<< " to physical address 0x" << std::hex << addr_x << std::endl;
 
 
         const uint8_t* data = pkt->getConstPtr<uint8_t>();
 
         Addr addr_a, addr_b, addr_c, addr_d;
         memcpy(&addr_a, data, sizeof(uint64_t));
         memcpy(&addr_b, data + 8, sizeof(uint64_t));
         memcpy(&addr_c, data + 16, sizeof(uint64_t));
         memcpy(&addr_d, data + 24, sizeof(uint64_t));
 
         uint8_t* hostAddr = pmemAddr;  // or whatever your backing pointer is
         Addr baseAddr = this->getAddrRange().start(); // base of memory range
 
         //std::cout << "[IMC] hostAddr = " << hostAddr << std::endl;
 
         //std::cout << "[IMC] addr_a = " << addr_a << ", addr_b = " << addr_b << ", addr_c = " << addr_c << std::endl;
 
         uint64_t val_a, val_b, val_c;
         memcpy(&val_a, hostAddr + (addr_a - baseAddr), sizeof(uint64_t));
         memcpy(&val_b, hostAddr + (addr_b - baseAddr), sizeof(uint64_t));
         memcpy(&val_c, hostAddr + (addr_c - baseAddr), sizeof(uint64_t));
 
         //std::cout << "[IMC] addr_a = 0x" << std::hex << addr_a << ", offset = 0x" << (addr_a - baseAddr) << std::endl;
         
         //std::cout << "*(hostAddr + offset) = 0x" << *(uint64_t*)(hostAddr + (addr_a - baseAddr)) << std::endl;
 
 
         //std::cout << "[IMC] a = " << val_a << ", b = " << val_b << ", c = " << val_c << std::endl;
 
         uint64_t result = (val_a * val_b) % val_c;
         memcpy(hostAddr + (addr_d - baseAddr), &result, sizeof(uint64_t));
 
         //std::cout << "[Mulmod IMC] a = " << val_a << ", b = " << val_b << ", c = " << val_c
           //        << ", result = " << result << std::endl;
 
         //if (pkt->isRead()) {
             //memcpy(pkt->getPtr<uint8_t>(), &result, sizeof(result));
         //}
 
         return true;
     }
 
     else if (pkt->req->getFlags() & Request::MULIMC) {
         std::cout << "[IMC] Performing in-memory compute for Mulimc" << std::endl;
 
         //Addr addr_x = 0x1b7c0;
         //uint64_t x_value = 1;
         //memcpy(pmemAddr + (addr_x - this->getAddrRange().start()), &x_value, sizeof(uint64_t));
         //std::cout << "[IMC] Wrote x = " << x_value
           //<< " to physical address 0x" << std::hex << addr_x << std::endl;
 
 
         const uint8_t* data = pkt->getConstPtr<uint8_t>();
 
         Addr addr_a, addr_b, addr_c;
         memcpy(&addr_a, data, sizeof(uint64_t));
         memcpy(&addr_b, data + 8, sizeof(uint64_t));
         memcpy(&addr_c, data + 16, sizeof(uint64_t));
 
         uint8_t* hostAddr = pmemAddr;  // or whatever your backing pointer is
         Addr baseAddr = this->getAddrRange().start(); // base of memory range
 
         //std::cout << "[IMC] hostAddr = " << hostAddr << std::endl;
 
         //std::cout << "[IMC] addr_a = " << addr_a << ", addr_b = " << addr_b << ", addr_c = " << addr_c << std::endl;
 
         uint64_t val_a, val_b;
         memcpy(&val_a, hostAddr + (addr_a - baseAddr), sizeof(uint64_t));
         memcpy(&val_b, hostAddr + (addr_b - baseAddr), sizeof(uint64_t));
 
         //std::cout << "[IMC] addr_a = 0x" << std::hex << addr_a << ", offset = 0x" << (addr_a - baseAddr) << std::endl;
         
         //std::cout << "*(hostAddr + offset) = 0x" << *(uint64_t*)(hostAddr + (addr_a - baseAddr)) << std::endl;
 
 
         //std::cout << "[IMC] a = " << val_a << ", b = " << val_b << ", c = " << val_c << std::endl;
 
         uint64_t result = val_a * val_b;
         memcpy(hostAddr + (addr_c - baseAddr), &result, sizeof(uint64_t));
 
         //std::cout << "[Mulimc IMC] a = " << val_a << ", b = " << val_b
           //        << ", result = " << result << std::endl;
 
         //if (pkt->isRead()) {
             //memcpy(pkt->getPtr<uint8_t>(), &result, sizeof(result));
         //}
 
         return true;
     }
 
     else if (pkt->req->getFlags() & Request::INVMOD) {
         std::cout << "[IMC] Performing in-memory compute for Invmod" << std::endl;
 
         //Addr addr_x = 0x1b7c0;
         //uint64_t x_value = 1;
         //memcpy(pmemAddr + (addr_x - this->getAddrRange().start()), &x_value, sizeof(uint64_t));
         //std::cout << "[IMC] Wrote x = " << x_value
           //<< " to physical address 0x" << std::hex << addr_x << std::endl;
 
 
         const uint8_t* data = pkt->getConstPtr<uint8_t>();
 
         Addr addr_a, addr_b, addr_c;
         memcpy(&addr_a, data, sizeof(uint64_t));
         memcpy(&addr_b, data + 8, sizeof(uint64_t));
         memcpy(&addr_c, data + 16, sizeof(uint64_t));
 
         uint8_t* hostAddr = pmemAddr;  // or whatever your backing pointer is
         Addr baseAddr = this->getAddrRange().start(); // base of memory range
 
         //std::cout << "[IMC] hostAddr = " << hostAddr << std::endl;
 
         //std::cout << "[IMC] addr_a = " << addr_a << ", addr_b = " << addr_b << ", addr_c = " << addr_c << std::endl;
 
         uint64_t val_a, val_b;
         memcpy(&val_a, hostAddr + (addr_a - baseAddr), sizeof(uint64_t));
         memcpy(&val_b, hostAddr + (addr_b - baseAddr), sizeof(uint64_t));
 
         //std::cout << "[IMC] addr_a = 0x" << std::hex << addr_a << ", offset = 0x" << (addr_a - baseAddr) << std::endl;
         
         //std::cout << "*(hostAddr + offset) = 0x" << *(uint64_t*)(hostAddr + (addr_a - baseAddr)) << std::endl;
 
 
         //std::cout << "[IMC] a = " << val_a << ", b = " << val_b << ", c = " << val_c << std::endl;
 
         uint64_t result;
         int m0 = val_b, t, q;
         int x0 = 0, x1 = 1;
         if (val_b == 1) {
             result = 0;
         }
         else {
             while (val_a > 1) {
                 q = val_a / val_b;
                 t = val_b;
                 val_b = val_a % val_b, val_a = t;
                 t = x0;
                 x0 = x1 - q * x0;
                 x1 = t;
             }
             if (x1 < 0) {
                 x1 += m0;
             }
             result = x1;
         }
 
 
         memcpy(hostAddr + (addr_c - baseAddr), &result, sizeof(uint64_t));
 
         //std::cout << "[Invmod IMC] a = " << val_a << ", b = " << val_b
           //        << ", result = " << result << std::endl;
 
         //if (pkt->isRead()) {
             //memcpy(pkt->getPtr<uint8_t>(), &result, sizeof(result));
         //}
 
         return true;
     }
 
     else if (pkt->req->getFlags() & Request::CMPIMC) {
         std::cout << "[IMC] Performing in-memory compute for Cmpimc" << std::endl;
 
         //Addr addr_x = 0x1b7c0;
         //uint64_t x_value = 1;
         //memcpy(pmemAddr + (addr_x - this->getAddrRange().start()), &x_value, sizeof(uint64_t));
         //std::cout << "[IMC] Wrote x = " << x_value
           //<< " to physical address 0x" << std::hex << addr_x << std::endl;
 
 
         const uint8_t* data = pkt->getConstPtr<uint8_t>();
 
         Addr addr_a, addr_b, addr_c;
         memcpy(&addr_a, data, sizeof(uint64_t));
         memcpy(&addr_b, data + 8, sizeof(uint64_t));
         memcpy(&addr_c, data + 16, sizeof(uint64_t));
 
         uint8_t* hostAddr = pmemAddr;  // or whatever your backing pointer is
         Addr baseAddr = this->getAddrRange().start(); // base of memory range
 
         //std::cout << "[IMC] hostAddr = " << hostAddr << std::endl;
 
         //std::cout << "[IMC] addr_a = " << addr_a << ", addr_b = " << addr_b << ", addr_c = " << addr_c << std::endl;
 
         uint64_t val_a, val_b;
         memcpy(&val_a, hostAddr + (addr_a - baseAddr), sizeof(uint64_t));
         memcpy(&val_b, hostAddr + (addr_b - baseAddr), sizeof(uint64_t));
 
         //std::cout << "[IMC] addr_a = 0x" << std::hex << addr_a << ", offset = 0x" << (addr_a - baseAddr) << std::endl;
         
         //std::cout << "*(hostAddr + offset) = 0x" << *(uint64_t*)(hostAddr + (addr_a - baseAddr)) << std::endl;
 
 
         //std::cout << "[IMC] a = " << val_a << ", b = " << val_b << ", c = " << val_c << std::endl;
 
         uint64_t result;
         if (val_a == val_b) {
             result = 1;
         } else {
             result = 0;
         }
 
 
         memcpy(hostAddr + (addr_c - baseAddr), &result, sizeof(uint64_t));
 
         //std::cout << "[Invmod IMC] a = " << val_a << ", b = " << val_b
           //        << ", result = " << result << std::endl;
 
         //if (pkt->isRead()) {
             //memcpy(pkt->getPtr<uint8_t>(), &result, sizeof(result));
         //}
 
         return true;
     }
 
     // go ahead and deal with the packet and put the response in the
     // queue if there is one
     bool needsResponse = pkt->needsResponse();
     recvAtomic(pkt);
     // turn packet around to go back to requestor if response expected
     if (needsResponse) {
         // recvAtomic() should already have turned packet into
         // atomic response
         assert(pkt->isResponse());
 
         Tick when_to_send = curTick() + receive_delay + getLatency();
 
         // typically this should be added at the end, so start the
         // insertion sort with the last element, also make sure not to
         // re-order in front of some existing packet with the same
         // address, the latter is important as this memory effectively
         // hands out exclusive copies (shared is not asserted)
         auto i = packetQueue.end();
         --i;
         while (i != packetQueue.begin() && when_to_send < i->tick &&
                !i->pkt->matchAddr(pkt))
             --i;
 
         // emplace inserts the element before the position pointed to by
         // the iterator, so advance it one step
         packetQueue.emplace(++i, pkt, when_to_send);
 
         if (!retryResp && !dequeueEvent.scheduled())
             schedule(dequeueEvent, packetQueue.back().tick);
     } else {
         pendingDelete.reset(pkt);
     }
 
     return true;
 }
 
 void
 PIMCtrl::release()
 {
     assert(isBusy);
     isBusy = false;
     if (retryReq) {
         retryReq = false;
         port.sendRetryReq();
     }
 }
 
 void
 PIMCtrl::dequeue()
 {
     assert(!packetQueue.empty());
     DeferredPacket deferred_pkt = packetQueue.front();
 
     retryResp = !port.sendTimingResp(deferred_pkt.pkt);
 
     if (!retryResp) {
         packetQueue.pop_front();
 
         // if the queue is not empty, schedule the next dequeue event,
         // otherwise signal that we are drained if we were asked to do so
         if (!packetQueue.empty()) {
             // if there were packets that got in-between then we
             // already have an event scheduled, so use re-schedule
             reschedule(dequeueEvent,
                        std::max(packetQueue.front().tick, curTick()), true);
         } else if (drainState() == DrainState::Draining) {
             DPRINTF(Drain, "Draining of PIMCtrl complete\n");
             signalDrainDone();
         }
     }
 }
 
 Tick
 PIMCtrl::getLatency() const
 {
     return latency +
         (latency_var ? random_mt.random<Tick>(0, latency_var) : 0);
 }
 
 void
 PIMCtrl::recvRespRetry()
 {
     assert(retryResp);
 
     dequeue();
 }
 
 Port &
 PIMCtrl::getPort(const std::string &if_name, PortID idx)
 {
     if (if_name != "port") {
         return AbstractMemory::getPort(if_name, idx);
     } else {
         return port;
     }
 }
 
 DrainState
 PIMCtrl::drain()
 {
     if (!packetQueue.empty()) {
         DPRINTF(Drain, "PIMCtrl Queue has requests, waiting to drain\n");
         return DrainState::Draining;
     } else {
         return DrainState::Drained;
     }
 }
 
 PIMCtrl::MemoryPort::MemoryPort(const std::string& _name,
    PIMCtrl& _memory)
     : ResponsePort(_name), mem(_memory)
 { }
 
 AddrRangeList
 PIMCtrl::MemoryPort::getAddrRanges() const
 {
     AddrRangeList ranges;
     ranges.push_back(mem.getAddrRange());
     return ranges;
 }
 
 Tick
 PIMCtrl::MemoryPort::recvAtomic(PacketPtr pkt)
 {
     return mem.recvAtomic(pkt);
 }
 
 Tick
 PIMCtrl::MemoryPort::recvAtomicBackdoor(
         PacketPtr pkt, MemBackdoorPtr &_backdoor)
 {
     return mem.recvAtomicBackdoor(pkt, _backdoor);
 }
 
 void
 PIMCtrl::MemoryPort::recvFunctional(PacketPtr pkt)
 {
     mem.recvFunctional(pkt);
 }
 
 void
 PIMCtrl::MemoryPort::recvMemBackdoorReq(const MemBackdoorReq &req,
         MemBackdoorPtr &backdoor)
 {
     mem.recvMemBackdoorReq(req, backdoor);
 }
 
 bool
 PIMCtrl::MemoryPort::recvTimingReq(PacketPtr pkt)
 {
     return mem.recvTimingReq(pkt);
 }
 
 void
 PIMCtrl::MemoryPort::recvRespRetry()
 {
     mem.recvRespRetry();
 }
 
 } // namespace memory
 } // namespace gem5 