#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    FBBBCharacterEquipmentRuntimeData &InEquipmentData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    ABBBCharacter &InCharacter,
    const FBBBCharacterEquipmentConfig &InEquipmentConfig)
{
    CharacterMesh = &InCharacterMesh;
    EquipmentData = &InEquipmentData;
    WorldData = &InWorldData;
    Character = &InCharacter;
    RightHandWeaponSocketName = InEquipmentConfig.RightHandWeaponSocketName;

    EquipmentData->Inventory.Slots.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.InventorySlotCount));

    EquipmentData->Inventory.QuickAccessBindings.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.QuickAccessSlotCount));

    DefaultEquipmentInitializer.Initialize(
        *EquipmentData,
        InCharacter,
        InEquipmentConfig);
}

void FBBBCharacterEquipmentSystem::AdvanceActions()
{
    if (!ensureMsgf(EquipmentData && WorldData, TEXT("[UBBBC]Equipment action advance dependencies are null")))
    {
        return;
    }

    ActionProcessor.Advance(
        WorldData->GetWorldTimeSeconds(),
        EquipmentData->Equipment);
}

//------------------------------------------------------------------------------

void FBBBCharacterEquipmentSystem::Shutdown()
{
    if (!EquipmentData)
    {
        return;
    }

    TSet<UBBBEquipmentInstance *> Instances;
    for (const TObjectPtr<UBBBEquipmentInstance> &Instance : EquipmentData->Inventory.Slots)
    {
        if (Instance)
        {
            Instances.Add(Instance);
        }
    }

    if (EquipmentData->Equipment.GetActiveMainHandInstance())
    {
        Instances.Add(EquipmentData->Equipment.GetActiveMainHandInstance());
    }

    if (EquipmentData->Equipment.GetDesiredMainHandInstance())
    {
        Instances.Add(EquipmentData->Equipment.GetDesiredMainHandInstance());
    }

    for (UBBBEquipmentInstance *Instance : Instances)
    {
        Instance->Shutdown();
    }
}

//------------------------------------------------------------------------------

void FBBBCharacterEquipmentSystem::Update()
{
    if (!ensureMsgf(EquipmentData && WorldData && CharacterMesh && Character, TEXT("[UBBBC]Equipment system update dependencies are null")))
    {
        return;
    }

    SelectionProcessor.Update(
        *Character,
        *CharacterMesh,
        RightHandWeaponSocketName,
        WorldData->GetWorldTimeSeconds(),
        EquipmentData->Commands,
        EquipmentData->Equipment,
        EquipmentData->Events);

    ActionProcessor.Update(
        WorldData->GetWorldTimeSeconds(),
        EquipmentData->Commands,
        EquipmentData->Equipment,
        EquipmentData->Events);
}

//------------------------------------------------------------------------------

void FBBBCharacterEquipmentSystem::UpdateAnimation()
{
    if (!ensureMsgf(EquipmentData && WorldData && CharacterMesh, TEXT("[UBBBC]Equipment animation update dependencies are null")))
    {
        return;
    }

    AnimationSystem.Update(
        *CharacterMesh,
        EquipmentData->Equipment,
        EquipmentData->Events,
        WorldData->GetWorldTimeSeconds());
}
