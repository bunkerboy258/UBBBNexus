#pragma once

#include "CoreMinimal.h"

struct FBBBRifleInputContext;

/** 步枪进入当前手持槽位的输入 */
struct FBBBRifleEquipInput final
{
    int32 Sequence = INDEX_NONE;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /** @param Context 解析上下文 @return 当前是否允许应用 */
    bool CanApply(const FBBBRifleInputContext &Context) const;

    /** @param Context 解析上下文 @return 无 */
    void Apply(FBBBRifleInputContext &Context) const;
};
