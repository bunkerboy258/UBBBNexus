#pragma once
#include "CoreMinimal.h"
class USkeletalMeshComponent;
struct FBBBCharacterAnimationCommands;

//消费蒙太奇队列
class ABBB_EVAC_API FBBBCharacterAnimationProcessor final
{
public:

    /**
     * 消费蒙太奇队列 逐条播放请求的蒙太奇
     * @param CharacterMesh	角色骨骼网格组件
     * @param AnimationCommands	动画命令
     */
    void Update(
        USkeletalMeshComponent &CharacterMesh,
        const FBBBCharacterAnimationCommands &AnimationCommands) const;
};
