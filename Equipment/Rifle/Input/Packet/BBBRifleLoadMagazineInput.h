#pragma once

#include "CoreMinimal.h"

struct FBBBRifleInputContext;

/** 步枪弹匣装入输入 */
struct FBBBRifleLoadMagazineInput final
{
    int32 Sequence = INDEX_NONE;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /** @param Context 解析上下文 @return 当前是否允许应用 */
    bool CanApply(const FBBBRifleInputContext &Context) const;

    /** @param Context 解析上下文 @return 无 */
    void Apply(FBBBRifleInputContext &Context) const;
};
