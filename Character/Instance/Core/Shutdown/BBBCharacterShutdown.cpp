#include "BBBWork/UBBBNexus/Character/Instance/Core/Shutdown/BBBCharacterShutdown.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"

void FBBBCharacterShutdown::Shutdown(ABBBCharacter &Character)
{
    Character.LateUpdateTick.SetTickFunctionEnable(false);

    USkeletalMeshComponent *CharacterMesh = Character.GetMesh();
    FBBBCharacterEquipmentRuntimeData &EquipmentData = Character.RuntimeData.Equipment;
    TSet<ABBBEquipment *> Instances;
    for (const TObjectPtr<ABBBEquipment> &Instance : EquipmentData.Inventory.Slots)
    {
        if (Instance)
        {
            Instances.Add(Instance);
        }
    }

    ABBBEquipment *ActiveInstance = EquipmentData.Equipment.GetActiveMainHandInstance();
    if (ActiveInstance)
    {
        Instances.Add(ActiveInstance);

    }

    if (ABBBEquipment *DesiredInstance = EquipmentData.Equipment.GetDesiredMainHandInstance())
    {
        Instances.Add(DesiredInstance);
    }

    if (UWorld *World = Character.GetWorld())
    {
        for (TActorIterator<ABBBEquipment> It(World); It; ++It)
        {
            if (It->GetOwner() == &Character)
            {
                Instances.Add(*It);
            }
        }
    }

    for (ABBBEquipment *Instance : Instances)
    {
        if (IsValid(Instance))
        {
            FBBBCharacterEquipmentLifecycleProcessor::Destroy(CharacterMesh, *Instance);
        }
    }
}
