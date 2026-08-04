#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentOperation.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

ABBBEquipmentActor *UBBBEquipmentOperation::Equip(
    UBBBItemInstance &ItemInstance,
    USkeletalMeshComponent &CharacterMesh,
    FBBBCharacterExternalAPI &CharacterAPI,
    FName AttachmentSocketName) const
{
    if (!ensureMsgf(
        EquipmentActorClass && CharacterMesh.GetOwner() && CharacterMesh.GetWorld(),
        TEXT("[UBBBI]Equipment actor creation dependencies are invalid")))
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = CharacterMesh.GetOwner();
    SpawnParameters.Instigator = Cast<APawn>(CharacterMesh.GetOwner());
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABBBEquipmentActor *EquipmentActor = CharacterMesh.GetWorld()->SpawnActor<ABBBEquipmentActor>(
        EquipmentActorClass,
        FTransform::Identity,
        SpawnParameters);

    if (!ensureMsgf(EquipmentActor, TEXT("[UBBBI]Equipment actor creation failed")))
    {
        return nullptr;
    }

    EquipmentActor->AttachToComponent(
        &CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        AttachmentSocketName);

    EquipmentActor->SetActorRelativeTransform(SpawnOffset);
    EquipmentActor->SetActorTickEnabled(ItemInstance.GetRuntimeData() != nullptr);
    EquipmentActor->Initialize(ItemInstance, CharacterAPI);
    EquipmentActor->Equip();
    return EquipmentActor;
}
