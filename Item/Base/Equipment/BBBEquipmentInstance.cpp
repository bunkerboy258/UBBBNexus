#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/RuntimeData/BBBItemRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

UBBBEquipmentInstance *UBBBEquipmentInstance::Create(
    UObject &Outer,
    UBBBEquipmentDefinition &InDefinition,
    int32 InStackCount)
{
    UBBBEquipmentInstance *Instance = NewObject<UBBBEquipmentInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBI]Equipment instance creation failed")))
    {
        return nullptr;
    }

    Instance->Configure(InDefinition, InStackCount);
    Instance->RuntimeData = NewObject<UBBBItemRuntimeData>(Instance);

    if (!ensureMsgf(Instance->RuntimeData, TEXT("[UBBBI]Equipment runtime data creation failed")))
    {
        return nullptr;
    }

    return Instance;
}

UBBBEquipmentInstance *UBBBEquipmentInstance::CreateMirror(
    UObject &Outer,
    UBBBEquipmentDefinition &InDefinition)
{
    UBBBEquipmentInstance *Instance = NewObject<UBBBEquipmentInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBI]Equipment mirror creation failed")))
    {
        return nullptr;
    }

    Instance->Configure(InDefinition, 1);
    Instance->InstanceMode = EInstanceMode::Mirror;
    return Instance;
}

void UBBBEquipmentInstance::Initialize(
    USkeletalMeshComponent &CharacterMesh,
    FBBBCharacterExternalAPI &CharacterAPI,
    FName AttachmentSocketName,
    float WorldTimeSeconds)
{
    UBBBEquipmentDefinition *EquipmentDefinition = GetEquipmentDefinition();
    if (!ensureMsgf(EquipmentDefinition, TEXT("[UBBBI]Equipment instance definition is invalid")))
    {
        return;
    }

    if (InstanceMode == EInstanceMode::Runtime && !bIsRuntimeDataInitialized)
    {
        if (!ensureMsgf(RuntimeData, TEXT("[UBBBI]Equipment runtime data is null")))
        {
            return;
        }

        RuntimeData->Initialize(*EquipmentDefinition);
        bIsRuntimeDataInitialized = true;
    }

    if (ModelActor)
    {
        return;
    }

    if (!ensureMsgf(
        EquipmentDefinition->EquipmentActorClass && CharacterMesh.GetOwner() && CharacterMesh.GetWorld(),
        TEXT("[UBBBI]Equipment model creation dependencies are invalid")))
    {
        return;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = CharacterMesh.GetOwner();
    SpawnParameters.Instigator = Cast<APawn>(CharacterMesh.GetOwner());
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ModelActor = CharacterMesh.GetWorld()->SpawnActor<ABBBEquipmentActor>(
        EquipmentDefinition->EquipmentActorClass,
        FTransform::Identity,
        SpawnParameters);

    if (!ensureMsgf(ModelActor, TEXT("[UBBBI]Equipment model actor creation failed")))
    {
        return;
    }

    ModelActor->AttachToComponent(
        &CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        AttachmentSocketName);

    ModelActor->SetActorRelativeTransform(EquipmentDefinition->SpawnOffset);
    ModelActor->SetActorTickEnabled(InstanceMode == EInstanceMode::Runtime);
    ModelActor->Initialize(*this, CharacterAPI);
    ModelActor->Equip();

    bIsEquipping = EquipmentDefinition->EquipMontage != nullptr;
    EquipEndTime = bIsEquipping
        ? WorldTimeSeconds + EquipmentDefinition->EquipMontage->GetPlayLength()
        : 0.0f;
}

void UBBBEquipmentInstance::ReleaseModel()
{
    if (ModelActor)
    {
        ModelActor->Destroy();
        ModelActor = nullptr;
    }

    bIsEquipping = false;
    EquipEndTime = 0.0f;
}

void UBBBEquipmentInstance::Update(float WorldTimeSeconds)
{
    if (bIsEquipping && WorldTimeSeconds >= EquipEndTime)
    {
        bIsEquipping = false;
        EquipEndTime = 0.0f;
    }
}

UBBBEquipmentDefinition *UBBBEquipmentInstance::GetEquipmentDefinition() const
{
    return Cast<UBBBEquipmentDefinition>(Definition);
}

ABBBEquipmentActor *UBBBEquipmentInstance::GetModelActor() const
{
    return ModelActor;
}

bool UBBBEquipmentInstance::IsEquipping() const
{
    return bIsEquipping;
}
