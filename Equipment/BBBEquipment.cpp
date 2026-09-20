#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentDefinition.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

ABBBEquipment::ABBBEquipment()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    PrimaryActorTick.EndTickGroup = TG_PostUpdateWork;
    SetActorEnableCollision(false);

    EquipmentRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("EquipmentRoot"));
    SetRootComponent(EquipmentRoot);

    EquipmentSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentSkeletalMesh"));
    EquipmentSkeletalMesh->SetupAttachment(EquipmentRoot);
    EquipmentSkeletalMesh->PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
    EquipmentSkeletalMesh->PrimaryComponentTick.EndTickGroup = TG_PostUpdateWork;
    EquipmentSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EquipmentSkeletalMesh->SetGenerateOverlapEvents(false);
    PrimaryActorTick.AddPrerequisite(EquipmentSkeletalMesh, EquipmentSkeletalMesh->PrimaryComponentTick);
}

const FGuid &ABBBEquipment::GetInstanceId() const
{
    return InstanceId;
}

FName ABBBEquipment::GetEquipmentId() const
{
    return Definition ? Definition->EquipmentId : NAME_None;
}

TSubclassOf<UAnimInstance> ABBBEquipment::GetCharacterAnimationLayerClass() const
{
    return Definition ? Definition->CharacterAnimationLayerClass : nullptr;
}

USkeletalMeshComponent *ABBBEquipment::GetEquipmentSkeletalMesh() const
{
    return EquipmentSkeletalMesh;
}

void ABBBEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateEquipment(DeltaSeconds);
}

void ABBBEquipment::SubmitCommand(const FBBBEquipmentCommand &)
{
}

void ABBBEquipment::UpdateEquipment(const float)
{
}

bool ABBBEquipment::InitializeEquipment(
    UBBBEquipmentDefinition &InDefinition,
    const FGuid &InInstanceId,
    const bool bInIsMirror)
{
    Definition = &InDefinition;
    InstanceId = InInstanceId;
    bIsMirror = bInIsMirror;

    if (!EquipmentSkeletalMesh)
    {
        return false;
    }

    EquipmentSkeletalMesh->SetSkeletalMesh(InDefinition.EquipmentMesh);
    EquipmentSkeletalMesh->SetAnimInstanceClass(InDefinition.EquipmentAnimationClass);
    return true;
}
