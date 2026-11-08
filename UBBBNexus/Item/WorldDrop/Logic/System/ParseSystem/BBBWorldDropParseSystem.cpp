#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/BBBWorldDropParseSystem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/Processors/BBBWorldDropInputProcessor.h"
void FBBBWorldDropParseSystem::Update(FBBBWorldDropRuntimeData &Data, bool bCausal) const
{
    FBBBWorldDropInputContext Context{Data.Drop.DropState, bCausal};
    auto &Slots = Data.Parse.InputState;
    FBBBWorldDropInputProcessor{}.Update(Slots, Context);
}
