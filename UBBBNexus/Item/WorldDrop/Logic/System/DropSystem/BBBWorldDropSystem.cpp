#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/BBBWorldDropSystem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/Context/BBBWorldDropUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/Processors/BBBWorldDropProcessor.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/Processors/BBBWorldDropPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
void FBBBWorldDropSystem::Update(ABBBWorldDropItem &Drop) const
{
    FBBBWorldDropUpdateContext Context{Drop, Drop.RuntimeData, Drop.HasAuthority()};
    FBBBWorldDropProcessor{}.Update(Context);
    FBBBWorldDropPresentationProcessor{}.Update(Context);
}
