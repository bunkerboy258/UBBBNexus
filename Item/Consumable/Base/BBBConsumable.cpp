#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Update/BBBConsumableUpdatePipeline.h"
#include "Components/StaticMeshComponent.h"

ABBBConsumable::ABBBConsumable()
{
    FBBBConsumableUpdatePipeline::ConfigureTick(*this);
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    SetRootComponent(ItemMesh);
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
    UpdateRuntimeData();
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

UBBBConsumableNetworkComponent *ABBBConsumable::GetNetworkComponent() const
{
    return NetworkComponent;
}
