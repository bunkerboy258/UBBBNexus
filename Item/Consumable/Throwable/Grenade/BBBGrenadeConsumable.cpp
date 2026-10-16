#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/Core/Initialization/BBBGrenadeInitializer.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/Core/Update/BBBGrenadeUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/Core/Shutdown/BBBGrenadeShutdown.h"
bool UBBBGrenadeConsumable::IsUsing() const
{
    return RuntimeData.Use.ReadGrenadeUseState().bUsing;
}
bool UBBBGrenadeConsumable::InitializeRuntimeData()
{
    return FBBBGrenadeInitializer{}.Initialize(*this);
}
void UBBBGrenadeConsumable::UpdateRuntimeData()
{
    FBBBGrenadeUpdatePipeline{}.Update(*this);
}
void UBBBGrenadeConsumable::ShutdownRuntimeData()
{
    FBBBGrenadeShutdown{}.Shutdown(*this);
}
void UBBBGrenadeConsumable::QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet)
{
    ParseSystem.Submit(FBBBGrenadePrimaryLocalControlPacket{{Packet.Stages}});
}
void UBBBGrenadeConsumable::QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet)
{
    ParseSystem.Submit(FBBBGrenadeFinishLocalControlPacket{});
}
