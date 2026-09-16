#include "BBBWork/UBBBNexus/Equipment/Core/Update/BBBEquipmentUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"

void FBBBEquipmentUpdatePipeline::Advance(
    ABBBEquipmentInstance &Instance,
    const float WorldTimeSeconds)
{
    Instance.ReloadSystem.Advance(Instance, WorldTimeSeconds);
}

void FBBBEquipmentUpdatePipeline::LateUpdate(
    ABBBEquipmentInstance &Instance,
    const float WorldTimeSeconds)
{
    if (!Instance.bIsActive)
    {
        return;
    }

    Instance.AnimationSystem.Update(Instance, WorldTimeSeconds);
}
