#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionInputState.h"

/** 手枪装入弹匣输入 */
struct FBBBPistolLoadAmmoLocalControlPacket final : public FBBBEquipmentLoadAmmoLocalControlPacket<FBBBPistolLoadAmmoLocalControlPacket>
{
    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入是否可写入本帧状态 */
    bool CanApply(const FBBBPistolActionState &State) const
    {
        return true;
    }

    /**
     * 将请求写入动作领域状态
     * @param State	手枪动作输入状态
     * @return 无
     */
    void Apply(FBBBPistolActionInputState &State) const
    {
        State.bLoadMagazineRequested = true;
    }
};
