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
    PrimaryActorTick.bCanEverTick = false;
    SetActorEnableCollision(false);
    SetActorHiddenInGame(true);

    EquipmentRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("EquipmentRoot"));
    SetRootComponent(EquipmentRoot);

    EquipmentSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentSkeletalMesh"));
    EquipmentSkeletalMesh->SetupAttachment(EquipmentRoot);
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

    HolderMesh = &InCharacterMesh;
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

bool ABBBEquipmentInstance::BeginEquipAction(
    const int32 Sequence,
    const float DurationOverride,
    FBBBEquipmentActionResult &OutResult)
{
    return EquipSystem.BeginAction(*this, Sequence, DurationOverride, OutResult);
}

bool ABBBEquipmentInstance::SubmitFire(
    const int32 Sequence,
    FBBBEquipmentActionResult &OutResult)
{
    return FireSystem.Fire(*this, Sequence, OutResult);
}

bool ABBBEquipmentInstance::SubmitReload(
    const float WorldTimeSeconds,
    const int32 Sequence,
    const float DurationOverride,
    FBBBEquipmentActionResult &OutResult)
{
    return ReloadSystem.Begin(*this, WorldTimeSeconds, Sequence, DurationOverride, OutResult);
}

void ABBBEquipmentInstance::AdvanceAction(const float WorldTimeSeconds)
{
    FBBBEquipmentUpdatePipeline::Advance(*this, WorldTimeSeconds);
}

void ABBBEquipmentInstance::PublishAnimationFacts(const float WorldTimeSeconds)
{
    FBBBEquipmentUpdatePipeline::LateUpdate(*this, WorldTimeSeconds);
}

bool ABBBEquipmentInstance::IsReloading() const
{
    return RuntimeData.Reload.bIsReloading;
}

float ABBBEquipmentInstance::GetEquipDuration() const
{
    return Definition ? FMath::Max(Definition->EquipConfig.EquipDuration, 0.01f) : 0.0f;
}

void ABBBEquipmentInstance::RecordAction(
    const EBBBEquipmentActionType Type,
    const int32 Sequence,
    const FBBBEquipmentActionResult &Result)
{
    AnimationSystem.RecordAction(Type, Sequence, Result);
}
