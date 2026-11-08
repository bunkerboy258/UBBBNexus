#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/Core/Initialization/BBBWorldDropInitializer.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/Core/Update/BBBWorldDropUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/Core/Shutdown/BBBWorldDropShutdown.h"
ABBBWorldDropItem::ABBBWorldDropItem()
{
    FBBBWorldDropInitializer::Construct(*this);
}
void ABBBWorldDropItem::BeginPlay()
{
    Super::BeginPlay();
    FBBBWorldDropInitializer::Initialize(*this);
}
void ABBBWorldDropItem::OnConstruction(const FTransform &Transform)
{
    Super::OnConstruction(Transform);
    FBBBWorldDropInitializer::Preview(*this);
}
void ABBBWorldDropItem::Tick(float DeltaSeconds)
{
    FBBBWorldDropUpdatePipeline::Update(*this);
}
void ABBBWorldDropItem::EndPlay(const EEndPlayReason::Type Reason)
{
    FBBBWorldDropShutdown::Shutdown(*this);
    Super::EndPlay(Reason);
}

bool ABBBWorldDropItem::ShouldTickIfViewportsOnly() const
{
    return true;
}
