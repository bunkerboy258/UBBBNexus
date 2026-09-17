#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Processors/BBBEquipmentReloadProcessor.h"

class ABBBEquipment;
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

/** 执行已批准的换弹阶段 */
class FBBBEquipmentReloadSystem final
{
private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

    /** 注入本系统所需的数据与行为配置 */
    void Initialize(FBBBEquipmentReloadRuntimeData &InData, FBBBEquipmentFireRuntimeData &InFire,
        const FBBBEquipmentReloadFragment &InFragment, FBBBCharacterExternalAPI &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror);

    /** 更新本系统 */
    void Update() const;

    FBBBEquipmentReloadRuntimeData *Data = nullptr;
    FBBBEquipmentFireRuntimeData *Fire = nullptr;
    const FBBBEquipmentReloadFragment *Fragment = nullptr;
    FBBBCharacterExternalAPI *CharacterAPI = nullptr;
    FName EquipmentId;
    bool bIsMirror = false;

    FBBBEquipmentReloadProcessor Processor;
};
