#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"
/** 本次使用的Primary请求 */
struct FBBBGrenadePrimaryLocalControlPacket final
    : public FBBBConsumablePrimaryLocalControlPacket<FBBBGrenadePrimaryLocalControlPacket>
{
    /** @return 格式有效 */
    bool IsValid() const
    {
        return !Stages.IsEmpty() && !Stages.ContainsByPredicate(
                                        [](uint8 Stage)
                                        {
                                            return Stage > 5;
                                        });
    }
    /** @param Inputs 使用输入 @return 允许解析 */
    bool CanApply(const FBBBGrenadeUseInputState &Inputs) const
    {
        return true;
    }
    /** @param Inputs 使用输入 @return 无 */
    void Apply(FBBBGrenadeUseInputState &Inputs) const
    {
        Inputs.Stage = Stages.Last();
        Inputs.bPrimary = true;
    }
};
