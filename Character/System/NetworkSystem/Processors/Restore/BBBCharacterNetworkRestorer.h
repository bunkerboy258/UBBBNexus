
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBAimRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBFireRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBReloadRestoreProcessor.h"
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentState;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBCharacterNetworkRestorer final
{
public:

    void Update(
        FBBBNetworkRuntimeData &NetworkData,
        FBBBAimRuntimeData &AimData,
        FBBBCharacterEquipmentState &EquipmentState) const;
private:

    //保存装备还原处理器供所属对象后续流程使用
    FBBBEquipmentRestoreProcessor EquipmentRestoreProcessor;

    //保存射击还原处理器供所属对象后续流程使用
    FBBBFireRestoreProcessor FireRestoreProcessor;

    //保存换弹还原处理器供所属对象后续流程使用
    FBBBReloadRestoreProcessor ReloadRestoreProcessor;

    //保存瞄准还原处理器供所属对象后续流程使用
    FBBBAimRestoreProcessor AimRestoreProcessor;
};
