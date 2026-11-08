#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionInputState.h"

/** 步枪装入弹匣输入 */
struct FBBBRifleLoadAmmoLocalControlPacket final : public FBBBEquipmentLoadAmmoLocalControlPacket<FBBBRifleLoadAmmoLocalControlPacket>
{
    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入是否可写入本帧状态 */
    bool CanApply(const FBBBRifleActionState &State) const
    {
        return true;
    }

    /**
     * 将请求写入动作领域状态
     * @param State	步枪动作输入状态
     * @return 无
     */
    void Apply(FBBBRifleActionInputState &State) const
    {
        State.bLoadMagazineRequested = true;
    }
};
