#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/DomainData/States/BBBHealthPackUseInputState.h"
/** 本次使用的Primary请求 */
struct FBBBHealthPackPrimaryLocalControlPacket final
    : public FBBBConsumablePrimaryLocalControlPacket<FBBBHealthPackPrimaryLocalControlPacket>
{
    /** @return 格式有效 */
    bool IsValid() const
    {
        return true;
    }
    /** @param Inputs 使用输入 @return 允许解析 */
    bool CanApply(const FBBBHealthPackUseInputState &Inputs) const
    {
        return true;
    }
    /** @param Inputs 使用输入 @return 无 */
    void Apply(FBBBHealthPackUseInputState &Inputs) const
    {
        Inputs.bPrimary = true;
    }
};
