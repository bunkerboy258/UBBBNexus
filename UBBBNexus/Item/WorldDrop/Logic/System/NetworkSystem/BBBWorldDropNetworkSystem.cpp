#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/NetworkSystem/BBBWorldDropNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/NetworkSystem/DomainData/Context/BBBWorldDropNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/NetworkSystem/Processors/BBBWorldDropNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
void FBBBWorldDropNetworkSystem::Update(ABBBWorldDropItem &Drop) const
{
    FBBBWorldDropNetworkUpdateContext Context{Drop, Drop.RuntimeData};
    FBBBWorldDropNetworkProcessor{}.Update(Context);
}
