#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Processors/BBBEquipmentEquipProcessor.h"

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

/** 装备挂接与人物装备表现系统 */
class FBBBEquipmentEquipSystem final
{
private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

    /** 注入本系统所需的数据与行为配置 */
    void Initialize(FBBBEquipmentEquipState &InData, const FBBBEquipmentFireState &InFire,
        const FBBBEquipmentEquipFragment &InFragment, ABBBCharacter &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror);

    /** 更新本系统 */
    void Update() const;

    FBBBEquipmentEquipState *Data = nullptr;
    const FBBBEquipmentFireState *Fire = nullptr;
    const FBBBEquipmentEquipFragment *Fragment = nullptr;
    ABBBCharacter *CharacterAPI = nullptr;
    FName EquipmentId;
    bool bIsMirror = false;

    FBBBEquipmentEquipProcessor Processor;
};
