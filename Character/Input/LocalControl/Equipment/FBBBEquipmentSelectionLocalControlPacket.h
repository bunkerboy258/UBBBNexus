#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/** 角色当前装备选择请求 */
struct FBBBEquipmentSelectionLocalControlPacket final
{
    /** 空标识表示空手 */
    FName EquipmentId = NAME_None;

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /** @param Context	本次输入上下文 @return 是否允许应用 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.EquipmentSelection.PendingEquipmentId = EquipmentId;
        Context.EquipmentSelection.bHasEquipmentRequest = true;
    }
};
