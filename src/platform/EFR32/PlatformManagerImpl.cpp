/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2019 Nest Labs, Inc.
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

/**
 *    @file
 *          Provides an implementation of the PlatformManager object
 *          for EFR32 platforms using the Silicon Labs EFR32 SDK.
 */
/* this file behaves like a config.h, comes first */
#include <platform/internal/CHIPDeviceLayerInternal.h>

#include <platform/PlatformManager.h>
#include <platform/internal/GenericPlatformManagerImpl_FreeRTOS.cpp>

#include <lwip/tcpip.h>

#include "AppConfig.h"
#include "FreeRTOS.h"

namespace chip {
namespace DeviceLayer {

PlatformManager & PlatformMgr()
{
    static PlatformManagerImpl sInstance;
    return sInstance;
}

CHIP_ERROR PlatformManagerImpl::InitChipStackInner()
{
    CHIP_ERROR err;

    // Initialize the configuration system.
    err = Internal::EFR32Config::Init();
    SuccessOrExit(err);

    // Initialize LwIP.
    tcpip_init(NULL, NULL);

    // Call _InitChipStack() on the generic implementation base class
    // to finish the initialization process.
    err = Internal::GenericPlatformManagerImpl_FreeRTOS::InitChipStackInner();
    SuccessOrExit(err);

exit:
    return err;
}

/*
 * The following Heap stats are compiled values done by the FreeRTOS Heap4 implementation.
 * See /examples/platform/efr32/heap_4_silabs.c
 * It keeps track of the number of calls to allocate and free memory as well as the
 * number of free bytes remaining, but says nothing about fragmentation.
 */

CHIP_ERROR PlatformManagerImpl::GetCurrentHeapFree(uint64_t & currentHeapFree)
{
    size_t freeHeapSize = xPortGetFreeHeapSize();
    currentHeapFree     = static_cast<uint64_t>(freeHeapSize);
    return CHIP_NO_ERROR;
}

CHIP_ERROR PlatformManagerImpl::GetCurrentHeapUsed(uint64_t & currentHeapUsed)
{
    // Calculate the Heap used based on Total heap - Free heap
    int64_t heapUsed = (configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize());

    // Something went wrong, this should not happen
    VerifyOrReturnError(heapUsed >= 0, CHIP_ERROR_INVALID_INTEGER_VALUE);
    currentHeapUsed = static_cast<uint64_t>(heapUsed);
    return CHIP_NO_ERROR;
}

CHIP_ERROR PlatformManagerImpl::GetCurrentHeapHighWatermark(uint64_t & currentHeapHighWatermark)
{
    // FreeRTOS records the lowest amount of available heap during runtime
    // currentHeapHighWatermark wants the highest heap usage point so we calculate it here
    int64_t HighestHeapUsageRecorded = (configTOTAL_HEAP_SIZE - xPortGetMinimumEverFreeHeapSize());

    // Something went wrong, this should not happen
    VerifyOrReturnError(HighestHeapUsageRecorded >= 0, CHIP_ERROR_INVALID_INTEGER_VALUE);
    currentHeapHighWatermark = static_cast<uint64_t>(HighestHeapUsageRecorded);

    return CHIP_NO_ERROR;
}

} // namespace DeviceLayer
} // namespace chip
