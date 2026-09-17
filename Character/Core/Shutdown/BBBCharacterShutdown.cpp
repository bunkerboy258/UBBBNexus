#include "BBBWork/UBBBNexus/Character/Core/Shutdown/BBBCharacterShutdown.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"

void FBBBCharacterShutdown::Shutdown(ABBBCharacterInstance &Character)
{
    Character.LateUpdateTick.SetTickFunctionEnable(false);

    USkeletalMeshComponent *CharacterMesh = Character.GetMesh();
    FBBBCharacterEquipmentRuntimeData &EquipmentData = Character.RuntimeData.Equipment;
    TSet<ABBBEquipmentInstance *> Instances;
    for (const TObjectPtr<ABBBEquipmentInstance> &Instance : EquipmentData.Inventory.Slots)
    {
        if (Instance)
        {
            Instances.Add(Instance);
        }
    }

    ABBBEquipmentInstance *ActiveInstance = EquipmentData.Equipment.GetActiveMainHandInstance();
    if (ActiveInstance)
    {
        Instances.Add(ActiveInstance);

    }

    if (ABBBEquipmentInstance *DesiredInstance = EquipmentData.Equipment.GetDesiredMainHandInstance())
    {
        Instances.Add(DesiredInstance);
    }

    if (UWorld *World = Character.GetWorld())
    {
        for (TActorIterator<ABBBEquipmentInstance> It(World); It; ++It)
        {
            if (It->GetOwner() == &Character)
            {
                Instances.Add(*It);
            }
        }
    }

    for (ABBBEquipmentInstance *Instance : Instances)
    {
        if (IsValid(Instance))
        {
            FBBBCharacterEquipmentLifecycleProcessor::Destroy(CharacterMesh, *Instance);
        }
    }
}
