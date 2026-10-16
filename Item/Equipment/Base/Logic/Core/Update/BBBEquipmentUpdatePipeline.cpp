#include "BBBWork/UBBBNexus/Item/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
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
    if (!Equipment.IsInitialized())
    {
        return;
    }

    // 真正卸下后才关闭装备 隐藏网格不改变持有关系
    if (!Equipment.IsEquipped())
    {
        Equipment.ShutdownRuntimeData();
        return;
    }

    UpdateInstance(Equipment);
}
