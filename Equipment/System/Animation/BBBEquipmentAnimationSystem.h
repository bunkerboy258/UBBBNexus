#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/Animation/States/BBBEquipmentAnimationStates.h"

class UBBBEquipmentInstance;
class UBBBEquipmentAnimInstance;
class USkeletalMeshComponent;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;

/** 在角色移动后统一采集并发布装备动画事实 */
class ABBB_EVAC_API FBBBEquipmentAnimationSystem final
{
public:
    /**
     * 计算并发布本帧装备动画快照
     * @param CharacterMesh	角色骨骼网格
     * @param EquipmentState	角色装备选择状态
     * @param EquipmentEvents	本帧装备动作事件
     * @param WorldTimeSeconds	本帧世界时间
     * @return 无
     */
    void Update(
        USkeletalMeshComponent &CharacterMesh,
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterEquipmentEvents &EquipmentEvents,
        float WorldTimeSeconds);

private:
    TWeakObjectPtr<UBBBEquipmentInstance> LastInstance;
    TWeakObjectPtr<UBBBEquipmentAnimInstance> LastAnimInstance;
    FBBBEquipmentAnimationFacts Facts;
};
