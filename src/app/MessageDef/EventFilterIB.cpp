/**
 *
 *    Copyright (c) 2021 Project CHIP Authors
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

#include "EventFilterIB.h"

#include "MessageDefHelper.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#include <app/AppBuildConfig.h>

namespace chip {
namespace app {
CHIP_ERROR EventFilterIB::Parser::Init(const chip::TLV::TLVReader & aReader)
{
    mReader.Init(aReader);
    VerifyOrReturnError(chip::TLV::kTLVType_Structure == mReader.GetType(), CHIP_ERROR_WRONG_TLV_TYPE);
    ReturnErrorOnFailure(mReader.EnterContainer(mOuterContainerType));
    return CHIP_NO_ERROR;
}

#if CHIP_CONFIG_IM_ENABLE_SCHEMA_CHECK
CHIP_ERROR EventFilterIB::Parser::CheckSchemaValidity() const
{
    CHIP_ERROR err      = CHIP_NO_ERROR;
    int TagPresenceMask = 0;
    TLV::TLVReader reader;

    PRETTY_PRINT("EventFilterIB =");
    PRETTY_PRINT("{");

    // make a copy of the reader
    reader.Init(mReader);

    while (CHIP_NO_ERROR == (err = reader.Next()))
    {
        VerifyOrReturnError(TLV::IsContextTag(reader.GetTag()), CHIP_ERROR_INVALID_TLV_TAG);
        uint32_t tagNum = TLV::TagNumFromTag(reader.GetTag());
        switch (tagNum)
        {
        case to_underlying(Tag::kNode):
            // check if this tag has appeared before
            VerifyOrReturnError(!(TagPresenceMask & (1 << to_underlying(Tag::kNode))), CHIP_ERROR_INVALID_TLV_TAG);
            TagPresenceMask |= (1 << to_underlying(Tag::kNode));
#if CHIP_DETAIL_LOGGING
            {
                NodeId node;
                ReturnLogErrorOnFailure(reader.Get(node));
                PRETTY_PRINT("\tNode = 0x%" PRIx64 ",", node);
            }
#endif // CHIP_DETAIL_LOGGING
            break;
        case to_underlying(Tag::kEventMin):
            // check if this tag has appeared before
            VerifyOrReturnError(!(TagPresenceMask & (1 << to_underlying(Tag::kEventMin))), CHIP_ERROR_INVALID_TLV_TAG);
            TagPresenceMask |= (1 << to_underlying(Tag::kEventMin));
#if CHIP_DETAIL_LOGGING
            {
                uint64_t eventMin;
                ReturnLogErrorOnFailure(reader.Get(eventMin));
                PRETTY_PRINT("\tEventMin = 0x%" PRIx64 ",", eventMin);
            }
#endif // CHIP_DETAIL_LOGGING
            break;
        default:
            PRETTY_PRINT("Unknown tag num %" PRIu32, tagNum);
            break;
        }
    }

    PRETTY_PRINT("},");
    PRETTY_PRINT("");

    if (CHIP_END_OF_TLV == err)
    {
        const int RequiredFields = (1 << to_underlying(Tag::kEventMin));

        if ((TagPresenceMask & RequiredFields) == RequiredFields)
        {
            err = CHIP_NO_ERROR;
        }
    }

    ReturnErrorOnFailure(err);
    ReturnErrorOnFailure(reader.ExitContainer(mOuterContainerType));
    return CHIP_NO_ERROR;
}
#endif // CHIP_CONFIG_IM_ENABLE_SCHEMA_CHECK

CHIP_ERROR EventFilterIB::Parser::GetNode(NodeId * const apNode) const
{
    return GetUnsignedInteger(to_underlying(Tag::kNode), apNode);
}

CHIP_ERROR EventFilterIB::Parser::GetEventMin(uint64_t * const apEventMin) const
{
    return GetUnsignedInteger(to_underlying(Tag::kEventMin), apEventMin);
}

CHIP_ERROR EventFilterIB::Builder::Init(TLV::TLVWriter * const apWriter)
{
    return InitAnonymousStructure(apWriter);
}

EventFilterIB::Builder & EventFilterIB::Builder::Node(const NodeId aNode)
{
    // skip if error has already been set
    if (mError == CHIP_NO_ERROR)
    {
        mError = mpWriter->Put(TLV::ContextTag(to_underlying(Tag::kNode)), aNode);
    }
    return *this;
}

EventFilterIB::Builder & EventFilterIB::Builder::EventMin(const uint64_t aEventMin)
{
    // skip if error has already been set
    if (mError == CHIP_NO_ERROR)
    {
        mError = mpWriter->Put(TLV::ContextTag(to_underlying(Tag::kEventMin)), aEventMin);
    }
    return *this;
}

EventFilterIB::Builder & EventFilterIB::Builder::EndOfEventFilterIB()
{
    EndOfContainer();
    return *this;
}
}; // namespace app
}; // namespace chip
