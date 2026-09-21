#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/**
 * 开火请求包 请求带内输给切枪与换弹
 */
struct FBBBFirePacket final
{

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /**
     * 登记开火事实并提交装备开火命令
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterInputContext &Context) const;
};
