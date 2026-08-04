#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Definition/BBBItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

UBBBItemInstance *UBBBItemInstance::Create(
    UObject &Outer,
    UBBBItemDefinition &InDefinition,
    int32 InStackCount)
{
    UBBBItemInstance *Instance = NewObject<UBBBItemInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBI]Item instance creation failed")))
    {
        return nullptr;
    }

    Instance->InstanceId = FGuid::NewGuid();
    Instance->Definition = &InDefinition;
    Instance->StackCount = FMath::Clamp(InStackCount, 1, InDefinition.MaxStack);
    Instance->RuntimeData = NewObject<UBBBItemRuntimeData>(Instance);

    if (!ensureMsgf(Instance->RuntimeData, TEXT("[UBBBI]Item runtime data creation failed")))
    {
        return nullptr;
    }

    Instance->RuntimeData->Initialize(InDefinition);
    return Instance;
}

UBBBItemInstance *UBBBItemInstance::CreateMirror(
    UObject &Outer,
    UBBBItemDefinition &InDefinition)
{
    UBBBItemInstance *Instance = NewObject<UBBBItemInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBI]Item mirror creation failed")))
    {
        return nullptr;
    }

    Instance->InstanceId = FGuid::NewGuid();
    Instance->Definition = &InDefinition;
    Instance->StackCount = 1;
    return Instance;
}

void UBBBItemInstance::Equip(
    USkeletalMeshComponent &CharacterMesh,
    FBBBCharacterExternalAPI &CharacterAPI,
    FName AttachmentSocketName)
{
    if (ModelActor)
    {
        return;
    }

    if (!ensureMsgf(
        Definition && Definition->EquipmentActorClass && CharacterMesh.GetOwner() && CharacterMesh.GetWorld(),
        TEXT("[UBBBI]Equipment actor creation dependencies are invalid")))
    {
        return;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = CharacterMesh.GetOwner();
    SpawnParameters.Instigator = Cast<APawn>(CharacterMesh.GetOwner());
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ModelActor = CharacterMesh.GetWorld()->SpawnActor<ABBBEquipmentActor>(
        Definition->EquipmentActorClass,
        FTransform::Identity,
        SpawnParameters);

    if (!ensureMsgf(ModelActor, TEXT("[UBBBI]Equipment actor creation failed")))
    {
        return;
    }

    ModelActor->AttachToComponent(
        &CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        AttachmentSocketName);

    ModelActor->SetActorRelativeTransform(Definition->SpawnOffset);
    ModelActor->SetActorTickEnabled(RuntimeData != nullptr);
    ModelActor->Initialize(*this, CharacterAPI);
    ModelActor->Equip();
}

void UBBBItemInstance::ReleaseModel()
{
    if (!ModelActor)
    {
        return;
    }

    ModelActor->Destroy();
    ModelActor = nullptr;
}

const FGuid &UBBBItemInstance::GetInstanceId() const
{
    return InstanceId;
}

UBBBItemDefinition *UBBBItemInstance::GetDefinition() const
{
    return Definition;
}

UBBBItemRuntimeData *UBBBItemInstance::GetRuntimeData() const
{
    return RuntimeData;
}

int32 UBBBItemInstance::GetStackCount() const
{
    return StackCount;
}

ABBBEquipmentActor *UBBBItemInstance::GetModelActor() const
{
    return ModelActor;
}

bool UBBBItemInstance::IsEquipping() const
{
    return ModelActor && ModelActor->IsEquipping();
}

bool UBBBItemInstance::IsValid() const
{
    return InstanceId.IsValid() && Definition != nullptr;
}
