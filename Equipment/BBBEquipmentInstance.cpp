#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Initialization/BBBEquipmentInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Update/BBBEquipmentUpdatePipeline.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ABBBEquipmentInstance::ABBBEquipmentInstance()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    PrimaryActorTick.EndTickGroup = TG_PostUpdateWork;
    SetActorEnableCollision(false);
    SetActorHiddenInGame(true);

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

ABBBEquipmentInstance *ABBBEquipmentInstance::Create(
    ABBBCharacterInstance &Holder,
    UBBBEquipmentDefinition &InDefinition,
    const bool bInIsMirror)
{
    UWorld *World = Holder.GetWorld();
    if (!ensureMsgf(World, TEXT("[UBBBE]Equipment creation has no world")))
    {
        return nullptr;
    }

    ABBBEquipmentInstance *Instance = World->SpawnActorDeferred<ABBBEquipmentInstance>(
        StaticClass(),
        FTransform::Identity,
        &Holder,
        &Holder,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (!ensureMsgf(Instance, TEXT("[UBBBE]Equipment actor creation failed")))
    {
        return nullptr;
    }

    Instance->PrimaryActorTick.AddPrerequisite(&Holder, Holder.PrimaryActorTick);
    Instance->ExternalAPI.Initialize(*Instance);
    Instance->InstanceId = FGuid::NewGuid();
    Instance->Definition = &InDefinition;
    Instance->bIsMirror = bInIsMirror;
    Instance->CharacterAPI = &Holder.GetExternalAPI();
    UGameplayStatics::FinishSpawningActor(Instance, FTransform::Identity);
    return Instance;
}

void ABBBEquipmentInstance::BeginPlay()
{
    Super::BeginPlay();
    FBBBEquipmentInitializer::Initialize(*this);
}

const FGuid &ABBBEquipmentInstance::GetInstanceId() const
{
    return InstanceId;
}

FName ABBBEquipmentInstance::GetEquipmentId() const
{
    return Definition ? Definition->EquipmentId : NAME_None;
}

TSubclassOf<UAnimInstance> ABBBEquipmentInstance::GetCharacterAnimationLayerClass() const
{
    return Definition ? Definition->CharacterAnimationLayerClass : nullptr;
}

USkeletalMeshComponent *ABBBEquipmentInstance::GetEquipmentSkeletalMesh() const
{
    return EquipmentSkeletalMesh;
}

bool ABBBEquipmentInstance::BindHolder(
    USkeletalMeshComponent &InCharacterMesh,
    const FName InAttachmentSocketName)
{
    if (!ensureMsgf(!InAttachmentSocketName.IsNone(), TEXT("[UBBBE]Equipment attachment socket is missing")))
    {
        return false;
    }

    if (USkeletalMeshComponent *PreviousMesh = HolderMesh.Get())
    {
        PrimaryActorTick.RemovePrerequisite(PreviousMesh, PreviousMesh->PrimaryComponentTick);
    }
    HolderMesh = &InCharacterMesh;
    PrimaryActorTick.AddPrerequisite(&InCharacterMesh, InCharacterMesh.PrimaryComponentTick);
    AttachmentSocketName = InAttachmentSocketName;
    return true;
}

bool ABBBEquipmentInstance::Activate()
{
    return EquipSystem.Activate(*this);
}

void ABBBEquipmentInstance::Deactivate()
{
    EquipSystem.Deactivate(*this);
}

void ABBBEquipmentInstance::Shutdown()
{
    Deactivate();
    HolderMesh.Reset();
    AttachmentSocketName = NAME_None;
    RuntimeData.Reload.bIsReloading = false;
    Destroy();
}

void ABBBEquipmentInstance::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBEquipmentUpdatePipeline::Update(*this, GetWorld()->GetTimeSeconds());
    FBBBEquipmentUpdatePipeline::LateUpdate(*this, GetWorld()->GetTimeSeconds());
}
