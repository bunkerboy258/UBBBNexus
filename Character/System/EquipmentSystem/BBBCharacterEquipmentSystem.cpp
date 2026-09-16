#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"

void FBBBCharacterEquipmentSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    FBBBCharacterEquipmentRuntimeData &InEquipmentData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    ABBBCharacterInstance &InCharacter,
    const FBBBCharacterEquipmentConfig &InEquipmentConfig)
{
    CharacterMesh = &InCharacterMesh;
    EquipmentData = &InEquipmentData;
    WorldData = &InWorldData;
    Character = &InCharacter;
    RightHandWeaponSocketName = InEquipmentConfig.RightHandWeaponSocketName;
    DefaultEquipmentConfig = &InEquipmentConfig;

    EquipmentData->Inventory.Slots.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.InventorySlotCount));

    EquipmentData->Inventory.QuickAccessBindings.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.QuickAccessSlotCount));

}

void FBBBCharacterEquipmentSystem::AdvanceActions()
{
    if (!ensureMsgf(
        EquipmentData && WorldData && Character && DefaultEquipmentConfig,
        TEXT("[UBBBC]Equipment action advance dependencies are null")))
    {
        return;
    }

    if (!bDefaultEquipmentInitialized)
    {
        DefaultEquipmentInitializer.Initialize(*EquipmentData, *Character, *DefaultEquipmentConfig);
        bDefaultEquipmentInitialized = true;
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

    TSet<ABBBEquipmentInstance *> Instances;
    for (const TObjectPtr<ABBBEquipmentInstance> &Instance : EquipmentData->Inventory.Slots)
    {
        if (Instance)
        {
            Instances.Add(Instance);
        }
    }

    if (EquipmentData->Equipment.GetActiveMainHandInstance())
    {
        ABBBEquipmentInstance *ActiveInstance = EquipmentData->Equipment.GetActiveMainHandInstance();
        Instances.Add(ActiveInstance);

        USkeletalMeshComponent *WeaponMesh = ActiveInstance->GetEquipmentSkeletalMesh();
        if (CharacterMesh && WeaponMesh)
        {
            WeaponMesh->PrimaryComponentTick.RemovePrerequisite(
                CharacterMesh,
                CharacterMesh->PrimaryComponentTick);
        }

        UBBBAnimInstance *CharacterAnim = CharacterMesh
            ? Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance())
            : nullptr;
        if (CharacterAnim)
        {
            CharacterAnim->BindWeaponAnimInstance(nullptr);
        }
    }

    if (EquipmentData->Equipment.GetDesiredMainHandInstance())
    {
        Instances.Add(EquipmentData->Equipment.GetDesiredMainHandInstance());
    }

    if (Character && Character->GetWorld())
    {
        for (TActorIterator<ABBBEquipmentInstance> It(Character->GetWorld()); It; ++It)
        {
            if (It->GetOwner() == Character)
            {
                Instances.Add(*It);
            }
        }
    }

    for (ABBBEquipmentInstance *Instance : Instances)
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

    ABBBEquipmentInstance *ActiveInstance = EquipmentData->Equipment.GetActiveMainHandInstance();
    if (ActiveInstance)
    {
        ActiveInstance->PublishAnimationFacts(WorldData->GetWorldTimeSeconds());
    }
}
