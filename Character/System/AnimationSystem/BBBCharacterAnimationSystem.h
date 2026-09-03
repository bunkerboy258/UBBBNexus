#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationFactProcessor.h"

class ABBBCharacter;
struct FBBBCharacterRuntimeData;
struct FBBBAnimationRuntimeData;
struct FBBBCharacterAnimationConfig;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterWorldRuntimeData;
class FBBBCharacterInitializer;
class USkeletalMeshComponent;

//角色动画系统
class ABBB_EVAC_API FBBBCharacterAnimationSystem final
{
public:
    /** 每帧播放瞬时动画命令并提交动画实例事实快照 */
    void Update();

    /**
     * 请求链接装备指定的动画层，空类恢复角色默认动画层
     * @param AnimationLayerClass 要链接的动画层类
     */
    void SetLinkedAnimationLayerClass(TSubclassOf<UAnimInstance> AnimationLayerClass);

private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化动画系统依赖与配置
     * @param InCharacter		角色
     * @param InRuntimeData	角色运行时数据
     * @param InCharacterMesh	角色骨骼网格组件
     * @param InAnimationData	动画运行时数据
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
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBCharacterAnimationConfig &InAnimationConfig);

    /** 根据当前装备刷新角色链接动画层 */
    void RefreshLinkedAnimationLayer();

    ABBBCharacter *Character = nullptr;
    FBBBCharacterRuntimeData *RuntimeData = nullptr;
    FBBBAnimationRuntimeData *AnimationData = nullptr;
    const FBBBCharacterEquipmentEvents *EquipmentEvents = nullptr;
    USkeletalMeshComponent *CharacterMesh = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    const FBBBCharacterAnimationConfig *AnimationConfig = nullptr;
    FBBBCharacterAnimationActionProcessor ActionProcessor;
    FBBBCharacterAnimationFactProcessor FactProcessor;
};
