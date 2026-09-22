#pragma once

#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/States/BBBRifleActionState.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/States/BBBRifleInputState.h"

/** 步枪状态的唯一直接持有者 */
struct FBBBRifleDomainState final
{
    /** @return 步枪动作事实的只读视图 */
    const FBBBRifleActionState &ReadRifleActionState() const
    {
        return Action;
    }

    /** @return 步枪待解析输入的只读视图 */
    const FBBBRifleInputState &ReadRifleInputState() const
    {
        return Input;
    }

private:
    friend class ABBBRifleEquipment;
    friend struct FBBBRifleFireInput;
    friend struct FBBBRifleReloadInput;
    friend struct FBBBRifleDetachMagazineInput;
    friend struct FBBBRifleLoadMagazineInput;
    friend struct FBBBRifleInterruptReloadInput;
    friend struct FBBBRifleRestoreFactInput;

    /** 输入包维护的动作事实 */
    FBBBRifleActionState Action;

    /** 装备根维护的输入槽与镜像队列 */
    FBBBRifleInputState Input;
};
