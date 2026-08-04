#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"

void FBBBCharacterNetworkSystem::Initialize(
    FBBBNetworkRuntimeData &InNetworkData,
    FBBBAimRuntimeData &InAimData,
    FBBBCharacterEquipmentState &InEquipmentState,
    UBBBCharacterNetworkComponent &InNetworkComponent,
    UBBBEquipmentCatalog &InEquipmentCatalog,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    FBBBCharacterEquipmentCommands &InEquipmentCommands,
    const FBBBCharacterEquipmentResults &InEquipmentResults,
    const FBBBAimConfig &InAimConfig)
{
    NetworkData = &InNetworkData;
    AimData = &InAimData;
    EquipmentState = &InEquipmentState;
    NetworkComponent = &InNetworkComponent;
    EquipmentCatalog = &InEquipmentCatalog;
    WorldData = &InWorldData;
    EquipmentCommands = &InEquipmentCommands;
    EquipmentResults = &InEquipmentResults;
    AimConfig = &InAimConfig;
}

//检查依赖
bool FBBBCharacterNetworkSystem::HasRequiredDependencies() const
{
    return ensureMsgf(
        NetworkData && WorldData && AimData && EquipmentCommands && EquipmentResults && EquipmentState && AimConfig && NetworkComponent && EquipmentCatalog,
        TEXT("[UBBBC]Network system operation aborted because dependencies are null"));
}

void FBBBCharacterNetworkSystem::UpdateValidation()
{
    if (!HasRequiredDependencies())
    { return; }

    Validator.Update(*NetworkData, *NetworkComponent);
}

void FBBBCharacterNetworkSystem::UpdateRestore()
{
    if (!HasRequiredDependencies())
    { return; }

    Restorer.Update(
        *NetworkData,
        *AimData,
        *EquipmentState,
        *EquipmentCommands,
        *EquipmentCatalog,
        *NetworkComponent);
}

void FBBBCharacterNetworkSystem::UpdateUpload()
{
    if (!HasRequiredDependencies())
    { return; }

    Uploader.Update(
        *NetworkData,
        WorldData->GetWorldTimeSeconds(),
        *AimData,
        *AimConfig,
        *EquipmentState,
        *EquipmentResults,
        *NetworkComponent);
}
