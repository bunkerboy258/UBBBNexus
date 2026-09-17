#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/Processors/BBBEquipmentEquipProcessor.h"

class ABBBEquipmentInstance;
class FBBBCharacterExternalAPI;
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

/** 装备挂接与人物装备表现系统 */
class FBBBEquipmentEquipSystem final
{
private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

    /** 注入本系统所需的数据与行为配置 */
    void Initialize(FBBBEquipmentEquipRuntimeData &InData, const FBBBEquipmentFireRuntimeData &InFire,
        const FBBBEquipmentEquipFragment &InFragment, FBBBCharacterExternalAPI &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror);

    /** 更新本系统 */
    void Update() const;

    FBBBEquipmentEquipRuntimeData *Data = nullptr;
    const FBBBEquipmentFireRuntimeData *Fire = nullptr;
    const FBBBEquipmentEquipFragment *Fragment = nullptr;
    FBBBCharacterExternalAPI *CharacterAPI = nullptr;
    FName EquipmentId;
    bool bIsMirror = false;

    FBBBEquipmentEquipProcessor Processor;
};
