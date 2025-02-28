/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
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
 *          for Zephyr platforms.
 */

#if !CONFIG_NORDIC_SECURITY_BACKEND
#include <crypto/CHIPCryptoPAL.h> // nogncheck
#endif                            // !CONFIG_NORDIC_SECURITY_BACKEND

#include <platform/internal/CHIPDeviceLayerInternal.h>

#include <lib/support/logging/CHIPLogging.h>
#include <platform/PlatformManager.h>
#include <platform/internal/GenericPlatformManagerImpl_Zephyr.cpp>

#include <drivers/entropy.h>
#include <malloc.h>

namespace chip {
namespace DeviceLayer {

static K_THREAD_STACK_DEFINE(sChipThreadStack, CHIP_DEVICE_CONFIG_CHIP_TASK_STACK_SIZE);

PlatformManagerImpl PlatformManagerImpl::sInstance{ sChipThreadStack };

#if !CONFIG_NORDIC_SECURITY_BACKEND
static int app_entropy_source(void * data, unsigned char * output, size_t len, size_t * olen)
{
    const struct device * entropy = device_get_binding(DT_CHOSEN_ZEPHYR_ENTROPY_LABEL);
    int ret                       = entropy_get_entropy(entropy, output, len);

    if (ret == 0)
    {
        *olen = len;
    }
    else
    {
        *olen = 0;
    }

    return ret;
}
#endif // !CONFIG_NORDIC_SECURITY_BACKEND

CHIP_ERROR PlatformManagerImpl::_InitChipStack(void)
{
    CHIP_ERROR err;

#if !CONFIG_NORDIC_SECURITY_BACKEND
    // Minimum required from source before entropy is released ( with mbedtls_entropy_func() ) (in bytes)
    const size_t kThreshold = 16;
#endif // !CONFIG_NORDIC_SECURITY_BACKEND

    // Initialize the configuration system.
    err = Internal::ZephyrConfig::Init();
    SuccessOrExit(err);

#if !CONFIG_NORDIC_SECURITY_BACKEND
    // Add entropy source based on Zephyr entropy driver
    err = chip::Crypto::add_entropy_source(app_entropy_source, NULL, kThreshold);
    SuccessOrExit(err);
#endif // !CONFIG_NORDIC_SECURITY_BACKEND

    // Call _InitChipStack() on the generic implementation base class to finish the initialization process.
    err = Internal::GenericPlatformManagerImpl_Zephyr<PlatformManagerImpl>::_InitChipStack();
    SuccessOrExit(err);

exit:
    return err;
}

CHIP_ERROR PlatformManagerImpl::_GetCurrentHeapFree(uint64_t & currentHeapFree)
{
#ifdef CONFIG_NEWLIB_LIBC
    // This will return the amount of memory which has been allocated from the system, but is not
    // used right now. Ideally, this value should be increased by the amount of memory which can
    // be allocated from the system, but Zephyr does not expose that number.
    currentHeapFree = mallinfo().fordblks;
    return CHIP_NO_ERROR;
#else
    return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
#endif
}

CHIP_ERROR PlatformManagerImpl::_GetCurrentHeapUsed(uint64_t & currentHeapUsed)
{
#ifdef CONFIG_NEWLIB_LIBC
    currentHeapUsed = mallinfo().uordblks;
    return CHIP_NO_ERROR;
#else
    return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
#endif
}

CHIP_ERROR PlatformManagerImpl::_GetCurrentHeapHighWatermark(uint64_t & currentHeapHighWatermark)
{
#ifdef CONFIG_NEWLIB_LIBC
    // ARM newlib does not provide a way to obtain the peak heap usage, so for now just return
    // the amount of memory allocated from the system which should be an upper bound of the peak
    // usage provided that the heap is not very fragmented.
    currentHeapHighWatermark = mallinfo().arena;
    return CHIP_NO_ERROR;
#else
    return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
#endif
}

} // namespace DeviceLayer
} // namespace chip
