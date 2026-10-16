#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterQuickMeleeContactProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterUseProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterUseReleaseProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterUseFinishProcessor.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionVisibilityProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionActionPermissionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionSelectionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionLifecycleProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionInputProcessor.h"

class FBBBCharacterInitializer;
class FBBBCharacterUpdatePipeline;
class ABBBCharacter;
class ABBBEquipment;
class USkeletalMeshComponent;
struct FBBBCharacterInteractionConfig;
struct FBBBCharacterRuntimeData;

/** 角色装备容器 选择与动作的唯一逻辑系统 */
class ABBB_EVAC_API FBBBCharacterInteractionSystem final
{
public:
private:
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterUpdatePipeline;
    friend class FBBBCharacterShutdown;

    /** @return 无 解除实际装备关系并清理镜像实例 */
    void Shutdown();

    /**
     * 维护装备选择并将本帧命令转发至装备 API
     * @param NetworkIdentity    本帧角色网络身份事实
     * @return 无
     */
    void Update();

    /** @return 无 在攀爬结果成立后交接装备操作许可 */
    void UpdateUsage();

    /**
     * 注入装备系统依赖并建立空容器
     * @param InCharacterMesh	角色骨骼网格
     * @param InEquipmentData	角色装备黑板
     * @param InCharacter	装备实例生命周期所有者
     * @param InInteractionConfig	角色持有物品交互配置
     * @return 无
     */
    void Initialize(
        USkeletalMeshComponent &InCharacterMesh,
        FBBBCharacterRuntimeData &InRuntimeData,
        ABBBCharacter &InCharacter,
        const FBBBCharacterInteractionConfig &InInteractionConfig);

    /** 角色装备黑板 */
    FBBBCharacterRuntimeData *RuntimeData = nullptr;

    /** 角色骨骼网格 */
    USkeletalMeshComponent *CharacterMesh = nullptr;

    /** 装备实例生命周期所有者 */
    ABBBCharacter *Character = nullptr;

    /** 右手装备挂接插槽 */
    FName RightHandSocketName = NAME_None;

    FBBBCharacterQuickMeleeContactProcessor QuickMeleeContactProcessor;
    FBBBCharacterUseProcessor ConsumableUseProcessor;
    FBBBCharacterUseReleaseProcessor ConsumableReleaseProcessor;
    FBBBCharacterUseFinishProcessor ConsumableFinishProcessor;
    /** 装备选择处理器 */
    FBBBCharacterInteractionSelectionProcessor SelectionProcessor;

    /** 装备生命周期处理器 */
    FBBBCharacterInteractionLifecycleProcessor LifecycleProcessor;

    /** 动画通知通信处理器 */
    FBBBCharacterInteractionInputProcessor AnimationInputProcessor;

    /** CMC 前的持有者操作许可通信 */
    FBBBCharacterInteractionActionPermissionProcessor ActionPermissionProcessor;
    /** 临时收起与恢复通信 */
    FBBBCharacterInteractionVisibilityProcessor VisibilityProcessor;

};
