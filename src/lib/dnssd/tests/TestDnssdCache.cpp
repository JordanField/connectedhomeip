/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// set this to 1 to enable DumpCache to see the state of the cache when needed
// #define MDNS_LOGGING 1
#include <cstdint>
#include <iostream>
#include <nlunit-test.h>

#include <inet/IPAddress.h>
#include <inet/InetInterface.h>
#include <inet/InetLayer.h>
#include <lib/core/CHIPError.h>
#include <lib/core/PeerId.h>
#include <lib/dnssd/DnssdCache.h>
#include <lib/support/UnitTestRegistration.h>
#include <lib/support/logging/CHIPLogging.h>
#include <system/SystemTimer.h>
#include <system/TimeSource.h>

#define NL_TEST_DEF_FN(fn) NL_TEST_DEF("Test " #fn, fn)

using namespace chip;
using namespace chip::Dnssd;

void TestCreate(nlTestSuite * inSuite, void * inContext)
{
    DnssdCache<10> tDnssdCache;
    tDnssdCache.DumpCache();
}

void TestInsert(nlTestSuite * inSuite, void * inContext)
{
    const int sizeOfCache = 5;
    DnssdCache<sizeOfCache> tDnssdCache;
    PeerId peerId;
    int64_t id                        = 0x100;
    uint16_t port                     = 2000;
    constexpr Inet::InterfaceId iface = Inet::InterfaceId::Null();

    Inet::IPAddress addr;
    Inet::IPAddress addrV6;
    const int ttl = 2; /* seconds */
    Inet::InterfaceId iface_out;
    Inet::IPAddress addr_out;
    uint16_t port_out;
    const uint64_t KNOWN_FABRIC = 0x100;

    Inet::IPAddress::FromString("1.0.0.1", addr);

    peerId.SetCompressedFabricId(KNOWN_FABRIC);

    for (uint16_t i = 0; i < 10; i++)
    {
        CHIP_ERROR result;

        // ml -- why doesn't adding 2 uint16_t give a uint16_t?
        peerId.SetNodeId((NodeId) id + i);
        result = tDnssdCache.Insert(peerId, addr, (uint16_t)(port + i), iface, 1000 * ttl);
        if (i < sizeOfCache)
        {
            NL_TEST_ASSERT(inSuite, result == CHIP_NO_ERROR);
        }
        else
        {
            NL_TEST_ASSERT(inSuite, result != CHIP_NO_ERROR);
        }
    }

    tDnssdCache.DumpCache();
    usleep(static_cast<useconds_t>((ttl + 1) * 1000 * 1000));
    id   = 0x200;
    port = 3000;
    for (uint16_t i = 0; i < sizeOfCache; i++)
    {
        CHIP_ERROR result;

        peerId.SetNodeId((NodeId) id + i);
        result = tDnssdCache.Insert(peerId, addr, (uint16_t)(port + i), iface, 1000 * ttl);
        NL_TEST_ASSERT(inSuite, result == CHIP_NO_ERROR);
    }
    tDnssdCache.DumpCache();

    for (uint16_t i = 0; i < sizeOfCache; i++)
    {
        CHIP_ERROR result;

        peerId.SetNodeId((NodeId) id + i);
        result = tDnssdCache.Delete(peerId);
        NL_TEST_ASSERT(inSuite, result == CHIP_NO_ERROR);
    }

    tDnssdCache.DumpCache();

    // ipv6 inserts
    Inet::IPAddress::FromString("::1", addrV6);
    port = 4000;
    for (uint16_t i = 0; i < sizeOfCache; i++)
    {
        CHIP_ERROR result;

        peerId.SetNodeId((NodeId) id + i);
        result = tDnssdCache.Insert(peerId, addrV6, (uint16_t)(port + i), iface, 1000 * ttl);
        NL_TEST_ASSERT(inSuite, result == CHIP_NO_ERROR);
    }

    tDnssdCache.DumpCache();

    NL_TEST_ASSERT(inSuite, tDnssdCache.Lookup(peerId, addr_out, port_out, iface_out) == CHIP_NO_ERROR);
    peerId.SetCompressedFabricId(KNOWN_FABRIC + 1);
    NL_TEST_ASSERT(inSuite, tDnssdCache.Lookup(peerId, addr_out, port_out, iface_out) != CHIP_NO_ERROR);
}

static const nlTest sTests[] = { NL_TEST_DEF_FN(TestCreate), NL_TEST_DEF_FN(TestInsert), NL_TEST_SENTINEL() };

int TestDnssdCache(void)
{
    nlTestSuite theSuite = { "MDNS Cache Creation", &sTests[0], nullptr, nullptr };

    nlTestRunner(&theSuite, nullptr);
    return nlTestRunnerStats(&theSuite);
}

CHIP_REGISTER_TEST_SUITE(TestDnssdCache)
