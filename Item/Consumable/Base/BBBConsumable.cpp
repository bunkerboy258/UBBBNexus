#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "Components/StaticMeshComponent.h"

ABBBConsumable::ABBBConsumable()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    SetRootComponent(ItemMesh);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ItemMesh->SetGenerateOverlapEvents(false);
    SetActorEnableCollision(false);
}

void ABBBConsumable::BeginPlay()
{
    Super::BeginPlay();
    bInitialized = InitializeRuntimeData();
    ensureMsgf(bInitialized, TEXT("消耗品初始化失败 %s"), *GetPathName());
}

void ABBBConsumable::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (bInitialized)
    {
        UpdateRuntimeData();
    }
}

void ABBBConsumable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bInitialized)
    {
        ShutdownRuntimeData();
    }
    Super::EndPlay(EndPlayReason);
}

UBBBConsumableDefinition *ABBBConsumable::GetDefinition() const
{
    return Definition;
}

bool ABBBConsumable::IsInitialized() const
{
    return bInitialized;
}

UStaticMeshComponent *ABBBConsumable::GetMesh() const
{
    return ItemMesh;
}

TSubclassOf<UAnimInstance> ABBBConsumable::GetCharacterAnimationLayerClass() const
{
    return Definition ? Definition->CharacterAnimationLayerClass : nullptr;
}

bool ABBBConsumable::TryGetAttachmentOffset(FTransform &OutOffset) const
{
    if (!Definition)
    {
        return false;
    }
    OutOffset = Definition->SpawnOffset;
    return !OutOffset.ContainsNaN();
}
