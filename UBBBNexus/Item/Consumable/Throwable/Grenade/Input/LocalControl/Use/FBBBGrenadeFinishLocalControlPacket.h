#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"
/** 本次使用的Finish请求 */
struct FBBBGrenadeFinishLocalControlPacket final
    : public FBBBConsumableFinishLocalControlPacket<FBBBGrenadeFinishLocalControlPacket>
{
    /** @return 格式有效 */
    bool IsValid() const
    {
        return true;
    }
    /** @param Inputs 使用输入 @return 允许解析 */
    bool CanApply(const FBBBGrenadeUseInputState &Inputs) const
    {
        return true;
    }
    /** @param Inputs 使用输入 @return 无 */
    void Apply(FBBBGrenadeUseInputState &Inputs) const
    {
        Inputs.bFinish = true;
    }
};
