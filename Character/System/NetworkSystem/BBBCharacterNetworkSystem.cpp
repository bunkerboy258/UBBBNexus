#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/World/BBBCharacterWorldRuntimeData.h"

void FBBBCharacterNetworkSystem::Initialize(
    FBBBNetworkRuntimeData &InNetworkData,
    FBBBAimRuntimeData &InAimData,
    FBBBCharacterEquipmentState &InEquipmentState,
    UBBBCharacterNetworkComponent &InNetworkComponent,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBCharacterItemEvents &InItemEvents,
    const FBBBAimConfig &InAimConfig)
{
    NetworkData = &InNetworkData;
    AimData = &InAimData;
    EquipmentState = &InEquipmentState;
    NetworkComponent = &InNetworkComponent;
    WorldData = &InWorldData;
    ItemEvents = &InItemEvents;
    AimConfig = &InAimConfig;
}

//检查依赖
bool FBBBCharacterNetworkSystem::HasRequiredDependencies() const
{
    return ensureMsgf(
        NetworkData && WorldData && AimData && ItemEvents && EquipmentState && AimConfig && NetworkComponent,
        TEXT("[UBBBC]Network system operation aborted because dependencies are null"));
}

void FBBBCharacterNetworkSystem::UpdateValidation()
{
    if (!HasRequiredDependencies())
    {
        return;
    }

    Validator.Update(*NetworkData, *NetworkComponent);
}

void FBBBCharacterNetworkSystem::UpdateRestore()
{
    if (!HasRequiredDependencies())
    {
        return;
    }

    Restorer.Update(*NetworkData, *AimData, *EquipmentState);
}

void FBBBCharacterNetworkSystem::UpdateUpload()
{
    if (!HasRequiredDependencies())
    {
        return;
    }

    Uploader.Update(
        *NetworkData,
        WorldData->GetWorldTimeSeconds(),
        *AimData,
        *AimConfig,
        *EquipmentState,
        *ItemEvents,
        *NetworkComponent);
}
