#pragma once

#include <cstdlib>

#include <app/CommandSender.h>
#include <app/InteractionModelDelegate.h>
#include <app/WriteClient.h>

namespace chip {
namespace Controller {
/**
 * @brief
 * Used for make current OnSuccessCallback & OnFailureCallback works when interaction model landed, it will be removed
 * after #6308 is landed.
 *
 * TODO:(#8967) Implementation of CommandSender::Callback should be removed after switching to ClusterObjects.
 */
class DeviceControllerInteractionModelDelegate : public chip::app::InteractionModelDelegate,
                                                 public chip::app::CommandSender::Callback,
                                                 public chip::app::WriteClient::Callback
{
public:
    void OnResponse(app::CommandSender * apCommandSender, const app::ConcreteCommandPath & aPath,
                    const chip::app::StatusIB & aStatus, TLV::TLVReader * aData) override;
    void OnError(const app::CommandSender * apCommandSender, const chip::app::StatusIB & aStatus,
                 CHIP_ERROR aProtocolError) override;
    void OnDone(app::CommandSender * apCommandSender) override;

    void OnResponse(const app::WriteClient * apWriteClient, const app::ConcreteAttributePath & aPath,
                    app::StatusIB attributeStatus) override;
    void OnError(const app::WriteClient * apWriteClient, CHIP_ERROR aError) override;
    void OnDone(app::WriteClient * apWriteClient) override;

    void OnReportData(const app::ReadClient * apReadClient, const app::ClusterInfo & aPath, TLV::TLVReader * apData,
                      Protocols::InteractionModel::Status status) override;
    CHIP_ERROR ReadError(app::ReadClient * apReadClient, CHIP_ERROR aError) override;

    CHIP_ERROR SubscribeResponseProcessed(const app::ReadClient * apSubscribeClient) override;

    CHIP_ERROR ReadDone(app::ReadClient * apReadClient) override;

    // TODO: FreeAttributePathParam and AllocateAttributePathParam are used by CHIPDevice.cpp for getting a long-live attribute path
    // object.
    void FreeAttributePathParam(uint64_t applicationId)
    {
        for (auto & item : mAttributePathTransactionMapPool)
        {
            if (item.ApplicationId == applicationId)
            {
                item.ApplicationId = UINT64_MAX;
            }
        }
    }

    // TODO: We only support allocating one path, should support multiple path later.
    app::AttributePathParams * AllocateAttributePathParam(size_t n, uint64_t applicationId)
    {
        if (n > 1)
        {
            return nullptr;
        }
        for (auto & item : mAttributePathTransactionMapPool)
        {
            if (item.ApplicationId == UINT64_MAX)
            {
                item.ApplicationId = applicationId;
                return &item.Params;
            }
        }
        return nullptr;
    }

private:
    struct AttributePathTransactionMap
    {
        uint64_t ApplicationId = UINT64_MAX;
        app::AttributePathParams Params;
    };
    AttributePathTransactionMap mAttributePathTransactionMapPool[CHIP_DEVICE_CONTROLLER_SUBSCRIPTION_ATTRIBUTE_PATH_POOL_SIZE];
};

} // namespace Controller
} // namespace chip
