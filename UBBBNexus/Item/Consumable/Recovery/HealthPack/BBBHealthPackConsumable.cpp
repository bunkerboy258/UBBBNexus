#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/BBBHealthPackConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/Core/Initialization/BBBHealthPackInitializer.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/Core/Update/BBBHealthPackUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/Core/Shutdown/BBBHealthPackShutdown.h"
bool UBBBHealthPackConsumable::IsUsing() const
{
    return RuntimeData.Use.ReadHealthPackUseState().bUsing;
}
bool UBBBHealthPackConsumable::InitializeRuntimeData()
{
    return FBBBHealthPackInitializer{}.Initialize(*this);
}
void UBBBHealthPackConsumable::UpdateRuntimeData()
{
    FBBBHealthPackUpdatePipeline{}.Update(*this);
}
void UBBBHealthPackConsumable::ShutdownRuntimeData()
{
    FBBBHealthPackShutdown{}.Shutdown(*this);
}
void UBBBHealthPackConsumable::QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet)
{
    ParseSystem.Submit(FBBBHealthPackPrimaryLocalControlPacket{});
}
void UBBBHealthPackConsumable::QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet)
{
    ParseSystem.Submit(FBBBHealthPackFinishLocalControlPacket{});
}
