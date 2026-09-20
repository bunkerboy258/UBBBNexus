#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/AnimationSystem/Processors/BBBEquipmentAnimationFactProcessor.h"

class ABBBEquipment;
class UWorld;
class USkeletalMeshComponent;
struct FBBBEquipmentInputRuntimeData;
struct FBBBEquipmentEquipState;
struct FBBBEquipmentFireState;
struct FBBBEquipmentReloadState;
struct FBBBEquipmentAnimationState;
struct FBBBEquipmentEquipFragment;
struct FBBBEquipmentFireFragment;
struct FBBBEquipmentReloadFragment;
class FBBBEquipmentInitializer;
class FBBBEquipmentUpdatePipeline;

/** 在角色移动后统一计算并发布单件装备动画事实 */
class FBBBEquipmentAnimationSystem final
{
private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

    /** 注入本系统所需的数据与行为配置 */
    void Initialize(USkeletalMeshComponent &InCharacterMesh, USkeletalMeshComponent &InWeaponMesh,
        FBBBEquipmentAnimationState &InData, const FBBBEquipmentFireState &InFire,
        const FBBBEquipmentReloadState &InReload,
        const FBBBEquipmentEquipFragment &InEquipFragment, const FBBBEquipmentFireFragment &InFireFragment);

    /** 更新本系统 */
    void Update() const;

    TWeakObjectPtr<USkeletalMeshComponent> CharacterMesh;
    USkeletalMeshComponent *WeaponMesh = nullptr;
    FBBBEquipmentAnimationState *Data = nullptr;
    const FBBBEquipmentFireState *Fire = nullptr;
    const FBBBEquipmentReloadState *Reload = nullptr;
    const FBBBEquipmentEquipFragment *EquipFragment = nullptr;
    const FBBBEquipmentFireFragment *FireFragment = nullptr;

    FBBBEquipmentAnimationFactProcessor Processor;
};
