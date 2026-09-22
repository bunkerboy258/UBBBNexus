#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterLocomotionUpdateContext;

/** 根据控制黑板驱动角色移动组件 */
class FBBBCharacterLocomotionProcessor final
{
public:
    /**
     * 应用本帧移动控制并发布移动状态
     * @param Context 本次移动更新上下文
     * @return 无
     */
    void Update(FBBBCharacterLocomotionUpdateContext &Context) const;
};
