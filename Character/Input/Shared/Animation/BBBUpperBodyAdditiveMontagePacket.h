#pragma once

#include "CoreMinimal.h"

class UAnimMontage;
struct FBBBCharacterInputContext;

/** UpperBodyAdditive 固定槽位的蒙太奇贡献 */
struct FBBBUpperBodyAdditiveMontagePacket final
{
    /** 待播放蒙太奇 空引用表示清除此槽 */
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** @return 输入是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @param Context	角色解析上下文 @return 是否允许贡献到固定槽位 */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /** @param Context	角色解析上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const;
};
