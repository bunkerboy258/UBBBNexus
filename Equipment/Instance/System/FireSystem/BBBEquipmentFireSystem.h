#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Processors/BBBEquipmentFireProcessor.h"

class ABBBEquipment;
class ABBBCharacter;
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

/** 单投射物开火与表现系统 */
class FBBBEquipmentFireSystem final
{
private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

    /** 注入本系统所需的数据与行为配置 */
    void Initialize(ABBBEquipment &InInstance, USkeletalMeshComponent &InWeaponMesh,
        FBBBEquipmentFireState &InData, const FBBBEquipmentReloadState &InReload,
        const FBBBEquipmentFireFragment &InFragment, ABBBCharacter &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror);

    /** 更新本系统 */
    void Update() const;

    ABBBEquipment *Instance = nullptr;
    USkeletalMeshComponent *WeaponMesh = nullptr;
    FBBBEquipmentFireState *Data = nullptr;
    const FBBBEquipmentReloadState *Reload = nullptr;
    const FBBBEquipmentFireFragment *Fragment = nullptr;
    ABBBCharacter *CharacterAPI = nullptr;
    FName EquipmentId;
    bool bIsMirror = false;

    FBBBEquipmentFireProcessor Processor;
};
