#pragma once

#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleDetachMagazineInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleEquipInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleFireInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleInterruptReloadInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleLoadMagazineInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleReloadInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleRestoreFactInput.h"
#include "Containers/StaticArray.h"

/** 步枪当前解析帧的固定输入集合 */
struct FBBBRifleInputFrame final
{
    static constexpr int32 MaxRestoreFactCount = 8;

    TStaticArray<FBBBRifleRestoreFactInput, MaxRestoreFactCount> RestoreFacts;

    int32 RestoreFactCount = 0;

    TBBBEquipmentInputSlot<FBBBRifleEquipInput> Equip;

    TBBBEquipmentInputSlot<FBBBRifleDetachMagazineInput> DetachMagazine;

    TBBBEquipmentInputSlot<FBBBRifleLoadMagazineInput> LoadMagazine;

    TBBBEquipmentInputSlot<FBBBRifleInterruptReloadInput> InterruptReload;

    TBBBEquipmentInputSlot<FBBBRifleReloadInput> Reload;

    TBBBEquipmentInputSlot<FBBBRifleFireInput> Fire;

    /**
     * 追加本帧待还原事实
     * @param Input 待还原事实输入
     * @return 是否成功写入固定队列
     */
    bool SubmitRestoreFact(const FBBBRifleRestoreFactInput &Input)
    {
        if (RestoreFactCount >= MaxRestoreFactCount)
        {
            return false;
        }

        RestoreFacts[RestoreFactCount++] = Input;
        return true;
    }

    /** 清除已经解析的全部输入 */
    void Reset()
    {
        RestoreFactCount = 0;
        Equip.Reset();
        DetachMagazine.Reset();
        LoadMagazine.Reset();
        InterruptReload.Reset();
        Reload.Reset();
        Fire.Reset();
    }
};
