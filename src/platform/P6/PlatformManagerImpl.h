/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    Copyright (c) 2018 Nest Labs, Inc.
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

/**
 *    @file
 *          Provides an implementation of the PlatformManager object
 *          for the PSoC6 platform.
 */

#pragma once

#include <platform/internal/GenericPlatformManagerImpl_FreeRTOS.h>

namespace chip {
namespace DeviceLayer {

/**
 * Concrete implementation of the PlatformManager singleton object for the PSoC6 platform.
 */
class PlatformManagerImpl final : public Internal::GenericPlatformManagerImpl_FreeRTOS
{
public:
    // ===== Platform-specific members that may be accessed directly by the application.

    CHIP_ERROR InitLwIPCoreLock(void);

private:
    // ===== Methods that implement the PlatformManager abstract interface.

    CHIP_ERROR InitChipStackInner(void) override;
};

} // namespace DeviceLayer
} // namespace chip
