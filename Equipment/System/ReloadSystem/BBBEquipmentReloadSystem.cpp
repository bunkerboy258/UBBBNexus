#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/BBBEquipmentReloadSystem.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentActionResult.h"

bool FBBBEquipmentReloadSystem::Begin(
    ABBBEquipmentInstance &Instance,
    const float WorldTimeSeconds,
    const int32 Sequence,
    const float DurationOverride,
    FBBBEquipmentActionResult &OutResult) const
{
    const UBBBEquipmentDefinition *Definition = Instance.Definition;
    if (!ensureMsgf(Instance.bIsActive && Definition, TEXT("[UBBBE]Equipment reload requires active equipment")))
    {
        return false;
    }

    FBBBEquipmentReloadRuntimeData &Reload = Instance.RuntimeData.Reload;
    if (Reload.bIsReloading)
    {
        return false;
    }

    const FBBBEquipmentReloadConfig &Config = Definition->ReloadConfig;
    Reload.bIsReloading = true;
    Reload.StartTimeSeconds = WorldTimeSeconds;
    Reload.DurationSeconds = DurationOverride > 0.0f
        ? DurationOverride
        : FMath::Max(Config.DurationSeconds, 0.01f);
    Reload.Sequence = Sequence;

    OutResult = FBBBEquipmentActionResult();
    OutResult.DurationSeconds = Reload.DurationSeconds;
    OutResult.Presentation.Montage = Config.Montage;
    Instance.RecordAction(EBBBEquipmentActionType::Reload, Sequence, OutResult);
    return true;
}

void FBBBEquipmentReloadSystem::Advance(
    ABBBEquipmentInstance &Instance,
    const float WorldTimeSeconds) const
{
    FBBBEquipmentReloadRuntimeData &Reload = Instance.RuntimeData.Reload;
    if (!Reload.bIsReloading)
    {
        return;
    }

    if (WorldTimeSeconds - Reload.StartTimeSeconds >= Reload.DurationSeconds)
    {
        Reload.bIsReloading = false;
    }
}
