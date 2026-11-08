#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/Processors/BBBWorldDropInputProcessor.h"
void FBBBWorldDropInputProcessor::Update(FBBBWorldDropInputState &Slots, FBBBWorldDropInputContext &Context) const
{
    Process(Slots.Initialize, Context);
    Process(Slots.Claim, Context);
    Process(Slots.Complete, Context);
    Process(Slots.Snapshot, Context);
}
