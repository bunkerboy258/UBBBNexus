#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterAnimationUpdateContext;

/** 根据装备黑板状态选择并链接角色动画层 */
class ABBB_EVAC_API FBBBCharacterAnimationLayerProcessor final
{
public:
    /**
     * 根据当前主手装备选择角色动画层
     * @param EquipmentState	装备黑板状态
     * @param AnimationConfig	角色动画配置
     * @param AnimationData	动画运行时数据
     * @param CharacterMesh	角色骨骼网格
     * @return 无
     */
    /**
     * 根据当前主手装备选择角色动画层
     * @param Context 本次动画更新上下文
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
