#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/Core/Update/BBBWorldDropUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/BBBWorldDropSystem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/NetworkSystem/BBBWorldDropNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/Core/Shutdown/BBBWorldDropShutdown.h"
void FBBBWorldDropUpdatePipeline::Update(ABBBWorldDropItem &Drop)
{
    FBBBWorldDropParseSystem{}.Update(Drop.RuntimeData, Drop.HasAuthority());
    FBBBWorldDropSystem{}.Update(Drop);
    FBBBWorldDropNetworkSystem{}.Update(Drop);
    if (Drop.HasAuthority() && Drop.ReadDropSnapshot().bConsumed)
    {
        FBBBWorldDropShutdown::Shutdown(Drop);
        Drop.Destroy();
    }
}
