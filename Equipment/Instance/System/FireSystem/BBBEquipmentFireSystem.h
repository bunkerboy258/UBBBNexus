#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Processors/BBBEquipmentFireProcessor.h"

class ABBBEquipment;
class ABBBCharacter;
class UWorld;
class USkeletalMeshComponent;
struct FBBBEquipmentInputRuntimeData;
struct FBBBEquipmentEquipRuntimeData;
struct FBBBEquipmentFireRuntimeData;
struct FBBBEquipmentReloadRuntimeData;
struct FBBBEquipmentAnimationRuntimeData;
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
        FBBBEquipmentFireRuntimeData &InData, const FBBBEquipmentReloadRuntimeData &InReload,
        const FBBBEquipmentFireFragment &InFragment, ABBBCharacter &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror);

    /** 更新本系统 */
    void Update() const;

    ABBBEquipment *Instance = nullptr;
    USkeletalMeshComponent *WeaponMesh = nullptr;
    FBBBEquipmentFireRuntimeData *Data = nullptr;
    const FBBBEquipmentReloadRuntimeData *Reload = nullptr;
    const FBBBEquipmentFireFragment *Fragment = nullptr;
    ABBBCharacter *CharacterAPI = nullptr;
    FName EquipmentId;
    bool bIsMirror = false;

    FBBBEquipmentFireProcessor Processor;
};
