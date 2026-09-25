#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/** 角色当前装备选择请求 */
struct FBBBEquipmentSelectionPacket final
{
    /** 空标识表示空手 */
    FName EquipmentId = NAME_None;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /** @param Context	本次输入上下文 @return 是否允许应用 */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const;
};
