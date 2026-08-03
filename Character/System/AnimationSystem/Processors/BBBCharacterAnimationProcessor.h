
#pragma once
#include "CoreMinimal.h"
class USkeletalMeshComponent;
struct FBBBAimRuntimeData;
struct FBBBCharacterAnimationCommands;
struct FBBBCharacterAnimationState;
struct FBBBFacingRuntimeData;
struct FBBBIntentRuntimeData;

//消费蒙太奇队列并更新表现开关
class ABBB_EVAC_API FBBBCharacterAnimationProcessor final
{
public:

    /**
     * 消费蒙太奇队列并更新开火与原地转身表现开关
     * @param CharacterMesh	角色骨骼网格组件
     * @param AnimationCommands	动画命令
     * @param AnimationState	输出的动画状态
     * @param IntentData	意图运行时数据
     * @param FacingData	朝向运行时数据
     * @param AimData	瞄准运行时数据
     */
    void Update(
        USkeletalMeshComponent &CharacterMesh,
        const FBBBCharacterAnimationCommands &AnimationCommands,
        FBBBCharacterAnimationState &AnimationState,
        const FBBBIntentRuntimeData &IntentData,
        const FBBBFacingRuntimeData &FacingData,
        const FBBBAimRuntimeData &AimData) const;
};
