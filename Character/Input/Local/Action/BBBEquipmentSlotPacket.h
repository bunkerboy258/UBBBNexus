#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/** 本机快捷栏装备选择请求 */
struct FBBBEquipmentSlotPacket final
{
    /** 空手使用 INDEX_NONE */
    int32 Slot = INDEX_NONE;

    /** @return 槽位格式是否合法 */
    bool IsValid() const;

    /** @param Context	本次输入上下文 @return 槽位是否存在 */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const;
};
