/**
 *
 *    Copyright (c) 2021 Project CHIP Authors
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
 *      This file defines EventFilterIB parser and builder in CHIP interaction model
 *
 */

#pragma once

#include "Builder.h"
#include "Parser.h"

#include <app/AppBuildConfig.h>
#include <app/util/basic-types.h>
#include <lib/core/CHIPCore.h>
#include <lib/core/CHIPTLV.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

namespace chip {
namespace app {
namespace EventFilterIB {
enum class Tag : uint8_t
{
    kNode     = 0,
    kEventMin = 1,
};

class Parser : public chip::app::Parser
{
public:
    /**
     *  @brief Initialize the parser object with TLVReader
     *
     *  @param [in] aReader A pointer to a TLVReader, which should point to the beginning of this EventFilterIB
     *
     *  @return #CHIP_NO_ERROR on success
     */
    CHIP_ERROR Init(const chip::TLV::TLVReader & aReader);

#if CHIP_CONFIG_IM_ENABLE_SCHEMA_CHECK
    /**
     *  @brief Roughly verify the message is correctly formed
     *   1) all mandatory tags are present
     *   2) all elements have expected data type
     *   3) any tag can only appear once
     *   4) At the top level of the structure, unknown tags are ignored for forward compatibility
     *  @note The main use of this function is to print out what we're
     *    receiving during protocol development and debugging.
     *    The encoding rule has changed in IM encoding spec so this
     *    check is only "roughly" conformant now.
     *
     *  @return #CHIP_NO_ERROR on success
     */
    CHIP_ERROR CheckSchemaValidity() const;
#endif

    /**
     *  @brief Get a TLVReader for the NodeId. Next() must be called before accessing them.
     *
     *  @param [in] apNodeId    A pointer to apNodeId
     *
     *  @return #CHIP_NO_ERROR on success
     *          #CHIP_ERROR_WRONG_TLV_TYPE if there is such element but it's not any of the defined unsigned integer types
     *          #CHIP_END_OF_TLV if there is no such element
     */
    CHIP_ERROR GetNode(NodeId * const apNodeId) const;

    /**
     *  @brief Get a TLVReader for the ClusterId. Next() must be called before accessing them.
     *
     *  @param [in] apEventMin    A pointer to EventMin
     *
     *  @return #CHIP_NO_ERROR on success
     *          #CHIP_ERROR_WRONG_TLV_TYPE if there is such element but it's not any of the defined unsigned integer types
     *          #CHIP_END_OF_TLV if there is no such element
     */
    CHIP_ERROR GetEventMin(uint64_t * const apEventMin) const;
};

class Builder : public chip::app::Builder
{
public:
    /**
     *  @brief Initialize a EventFilterIB::Builder for writing into a TLV stream
     *
     *  @param [in] apWriter    A pointer to TLVWriter
     *
     *  @return #CHIP_NO_ERROR on success
     */
    CHIP_ERROR Init(chip::TLV::TLVWriter * const apWriter);

    /**
     *  @brief Inject Node into the TLV stream to indicate the nodeId referenced by the event filter.
     *
     *  @param [in] aNodeId refer to the NodeId
     *
     *  @return A reference to *this
     */
    EventFilterIB::Builder & Node(const NodeId aNode);

    /**
     *  @brief Inject EventId into the TLV stream.
     *
     *  @param [in] aEventMin EventMin for this event filter
     *
     *  @return A reference to *this
     */
    EventFilterIB::Builder & EventMin(const uint64_t aEventMin);

    /**
     *  @brief Mark the end of this EventFilterIB
     *
     *  @return A reference to *this
     */
    EventFilterIB::Builder & EndOfEventFilterIB();
};
}; // namespace EventFilterIB
}; // namespace app
}; // namespace chip
