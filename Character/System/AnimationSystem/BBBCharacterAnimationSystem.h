#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationFactProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationLayerProcessor.h"

class ABBBCharacter;
struct FBBBCharacterRuntimeData;
struct FBBBAnimationRuntimeData;
struct FBBBCharacterAnimationConfig;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterWorldRuntimeData;
class FBBBCharacterInitializer;
class USkeletalMeshComponent;

//角色动画系统
class ABBB_EVAC_API FBBBCharacterAnimationSystem final
{
public:
    /** 每帧播放瞬时动画命令并提交动画实例事实快照 */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化动画系统依赖与配置
     * @param InCharacter		角色
     * @param InRuntimeData	角色运行时数据
     * @param InCharacterMesh	角色骨骼网格组件
     * @param InAnimationData	动画运行时数据
     * @param InEquipmentEvents	装备事件
     * @param InEquipmentState	装备状态
     * @param InWorldData		世界运行时数据
     * @param InAnimationConfig	动画配置
     */
    void Initialize(
        ABBBCharacter &InCharacter,
        FBBBCharacterRuntimeData &InRuntimeData,
        USkeletalMeshComponent &InCharacterMesh,
        FBBBAnimationRuntimeData &InAnimationData,
        const FBBBCharacterEquipmentEvents &InEquipmentEvents,
        const FBBBCharacterEquipmentState &InEquipmentState,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBCharacterAnimationConfig &InAnimationConfig);

    ABBBCharacter *Character = nullptr;
    FBBBCharacterRuntimeData *RuntimeData = nullptr;
    FBBBAnimationRuntimeData *AnimationData = nullptr;
    const FBBBCharacterEquipmentEvents *EquipmentEvents = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;
    USkeletalMeshComponent *CharacterMesh = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    const FBBBCharacterAnimationConfig *AnimationConfig = nullptr;
    FBBBCharacterAnimationActionProcessor ActionProcessor;
    FBBBCharacterAnimationFactProcessor FactProcessor;
    FBBBCharacterAnimationLayerProcessor LayerProcessor;
};
