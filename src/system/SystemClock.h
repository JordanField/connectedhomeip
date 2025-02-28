/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2018 Nest Labs, Inc.
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
 *      This is an internal header file that defines the interface to a platform-supplied
 *      function for retrieving the current system time.
 */

#pragma once

// Include configuration headers
#include <system/SystemConfig.h>

// Include dependent headers
#include <lib/support/DLLUtil.h>
#include <lib/support/TimeUtils.h>
#include <system/SystemError.h>

#if CHIP_SYSTEM_CONFIG_USE_POSIX_TIME_FUNCTS || CHIP_SYSTEM_CONFIG_USE_SOCKETS
#include <sys/time.h>
#endif // CHIP_SYSTEM_CONFIG_USE_POSIX_TIME_FUNCTS || CHIP_SYSTEM_CONFIG_USE_SOCKETS

#include <chrono>
#include <stdint.h>

namespace chip {
namespace System {

namespace Clock {

/*
 * We use `std::chrono::duration` for clock types to provide type safety. But unlike the predefined std types
 * (`std::chrono::milliseconds` et al), CHIP uses unsigned base types, and types are explicity sized, with
 * smaller-size types available for members and arguments where appropriate.
 *
 * Most conversions are handled by the types transparently. To convert with possible loss of information, use
 * `std::chrono::duration_cast<>()`.
 */

using Microseconds64 = std::chrono::duration<uint64_t, std::micro>;
using Microseconds32 = std::chrono::duration<uint32_t, std::micro>;

using Milliseconds64 = std::chrono::duration<uint64_t, std::milli>;
using Milliseconds32 = std::chrono::duration<uint32_t, std::milli>;

using Seconds64 = std::chrono::duration<uint64_t>;
using Seconds32 = std::chrono::duration<uint32_t>;
using Seconds16 = std::chrono::duration<uint16_t>;

constexpr Seconds16 Zero{ 0 };

namespace Literals {

constexpr Microseconds64 operator""_us(unsigned long long int us)
{
    return Microseconds64(us);
}
constexpr Microseconds64 operator""_us64(unsigned long long int us)
{
    return Microseconds64(us);
}
constexpr Microseconds32 operator""_us32(unsigned long long int us)
{
    return Microseconds32(us);
}

constexpr Milliseconds64 operator""_ms(unsigned long long int ms)
{
    return Milliseconds64(ms);
}
constexpr Milliseconds64 operator""_ms64(unsigned long long int ms)
{
    return Milliseconds64(ms);
}
constexpr Milliseconds32 operator""_ms32(unsigned long long int ms)
{
    return Milliseconds32(ms);
}

constexpr Seconds64 operator""_s(unsigned long long int s)
{
    return Seconds64(s);
}
constexpr Seconds64 operator""_s64(unsigned long long int s)
{
    return Seconds64(s);
}
constexpr Seconds32 operator""_s32(unsigned long long int s)
{
    return Seconds32(s);
}
constexpr Seconds16 operator""_s16(unsigned long long int s)
{
    return Seconds16(s);
}

} // namespace Literals

/**
 * Type for System time stamps.
 */
using Timestamp = Milliseconds64;

/**
 * Type for System time offsets (i.e. `StartTime()` duration).
 *
 * It is required of platforms that time stamps from `GetMonotonic…()` have the high bit(s) zero,
 * so the sum of a `Milliseconds64` time stamp and `Milliseconds32` offset will never overflow.
 */
using Timeout = Milliseconds32;

class ClockBase
{
public:
    // Backwards compatibility types; avoid using in new code. These only provide documentation, not type safety.
    using Tick = uint64_t;
    static_assert(std::is_unsigned<Tick>::value, "Tick must be unsigned");
    using MonotonicMicroseconds = Tick;
    using MonotonicMilliseconds = Tick;

    virtual ~ClockBase() = default;

    /**
     * Returns a monotonic system time.
     *
     * This function returns an elapsed time since an arbitrary, platform-defined epoch.
     * The value returned is guaranteed to be ever-increasing (i.e. never wrapping or decreasing) between
     * reboots of the system.  Additionally, the underlying time source is guaranteed to tick
     * continuously during any system sleep modes that do not entail a restart upon wake.
     *
     * Although some platforms may choose to return a value that measures the time since boot for the
     * system, applications must *not* rely on this.
     */
    Timestamp GetMonotonicTimestamp() { return GetMonotonicMilliseconds64(); }

    /**
     * Returns a monotonic system time in units of microseconds.
     *
     * This function returns an elapsed time in microseconds since an arbitrary, platform-defined epoch.
     * The value returned is guaranteed to be ever-increasing (i.e. never wrapping or decreasing) between
     * reboots of the system.  Additionally, the underlying time source is guaranteed to tick
     * continuously during any system sleep modes that do not entail a restart upon wake.
     *
     * Although some platforms may choose to return a value that measures the time since boot for the
     * system, applications must *not* rely on this.
     *
     * Applications must not rely on the time returned by GetMonotonicMicroseconds64() actually having
     * granularity finer than milliseconds.
     *
     * Platform implementations *must* use the same epoch for GetMonotonicMicroseconds64() and GetMonotonicMilliseconds64().
     *
     * Platforms *must* use an epoch such that the upper bit of a value returned by GetMonotonicMicroseconds64() is zero
     * for the expected operational life of the system.
     *
     * @returns Elapsed time in microseconds since an arbitrary, platform-defined epoch.
     */
    virtual Microseconds64 GetMonotonicMicroseconds64() = 0;

    /**
     * Returns a monotonic system time in units of milliseconds.
     *
     * This function returns an elapsed time in milliseconds since an arbitrary, platform-defined epoch.
     * The value returned is guaranteed to be ever-increasing (i.e. never wrapping or decreasing) between
     * reboots of the system.  Additionally, the underlying time source is guaranteed to tick
     * continuously during any system sleep modes that do not entail a restart upon wake.
     *
     * Although some platforms may choose to return a value that measures the time since boot for the
     * system, applications must *not* rely on this.
     *
     * Platform implementations *must* use the same epoch for GetMonotonicMicroseconds64() and GetMonotonicMilliseconds64().
     * (As a consequence of this, and the requirement for GetMonotonicMicroseconds64() to return high bit zero, values
     * returned by GetMonotonicMilliseconds64() will have the high ten bits zero.)
     *
     * @returns             Elapsed time in milliseconds since an arbitrary, platform-defined epoch.
     */
    virtual Milliseconds64 GetMonotonicMilliseconds64() = 0;

    // Backwards compatibility methods; avoid using in new code. These only provide documentation, not type safety.
    MonotonicMicroseconds GetMonotonicMicroseconds() { return GetMonotonicMicroseconds64().count(); }
    MonotonicMilliseconds GetMonotonicMilliseconds() { return GetMonotonicMilliseconds64().count(); }
};

// Currently we have a single implementation class, ClockImpl, whose members are implemented in build-specific files.
class ClockImpl : public ClockBase
{
public:
    ~ClockImpl() = default;
    Microseconds64 GetMonotonicMicroseconds64() override;
    Milliseconds64 GetMonotonicMilliseconds64() override;
};

namespace Internal {

// This should only be used via SystemClock() below.
extern ClockBase * gClockBase;

inline void SetSystemClockForTesting(Clock::ClockBase * clock)
{
    Clock::Internal::gClockBase = clock;
}

} // namespace Internal

// Backwards compatibility types; avoid using in new code. These only provide documentation, not type safety.
using MonotonicMicroseconds = ClockBase::MonotonicMicroseconds;
using MonotonicMilliseconds = ClockBase::MonotonicMilliseconds;

#if CHIP_SYSTEM_CONFIG_USE_POSIX_TIME_FUNCTS || CHIP_SYSTEM_CONFIG_USE_SOCKETS
Microseconds64 TimevalToMicroseconds(const timeval & in);
void ToTimeval(Microseconds64 in, timeval & out);
#endif // CHIP_SYSTEM_CONFIG_USE_POSIX_TIME_FUNCTS || CHIP_SYSTEM_CONFIG_USE_SOCKETS

} // namespace Clock

inline Clock::ClockBase & SystemClock()
{
    return *Clock::Internal::gClockBase;
}

} // namespace System
} // namespace chip
