#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBEquipmentUpdatePipeline::ConfigureTick(ABBBEquipment &Equipment)
{
    Equipment.PrimaryActorTick.bCanEverTick = true;
    Equipment.PrimaryActorTick.bStartWithTickEnabled = false;
    Equipment.PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    Equipment.PrimaryActorTick.EndTickGroup = TG_PostUpdateWork;

    if (USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh())
    {
        Equipment.PrimaryActorTick.AddPrerequisite(Mesh, Mesh->PrimaryComponentTick);
    }
}

void FBBBEquipmentUpdatePipeline::Update(ABBBEquipment &Equipment) const
{
    if (Equipment.IsInitialized())
    {
        UpdateInstance(Equipment);
    }
}
