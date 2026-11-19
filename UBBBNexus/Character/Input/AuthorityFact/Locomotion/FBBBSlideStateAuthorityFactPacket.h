#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/** 镜像角色接收的当前滑铲状态 */
struct FBBBSlideStateAuthorityFactPacket final
{
    /** 是否正在滑铲 */
    bool bSlide = false;

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
        Context.Locomotion.bSlide = bSlide;
    }
};
