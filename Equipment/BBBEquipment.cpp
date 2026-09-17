#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Update/BBBEquipmentUpdatePipeline.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

ABBBEquipment::ABBBEquipment()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    PrimaryActorTick.EndTickGroup = TG_PostUpdateWork;
    SetActorEnableCollision(false);

    EquipmentRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("EquipmentRoot"));
    SetRootComponent(EquipmentRoot);

    EquipmentSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentSkeletalMesh"));
    EquipmentSkeletalMesh->SetupAttachment(EquipmentRoot);
    EquipmentSkeletalMesh->PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
    EquipmentSkeletalMesh->PrimaryComponentTick.EndTickGroup = TG_PostUpdateWork;
    EquipmentSkeletalMesh->PrimaryComponentTick.AddPrerequisite(this, PrimaryActorTick);
    EquipmentSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EquipmentSkeletalMesh->SetGenerateOverlapEvents(false);
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
    UpdatePipeline.Update();
}
