#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/Core/Initialization/BBBGrenadeInitializer.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/Core/Update/BBBGrenadeUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/Core/Shutdown/BBBGrenadeShutdown.h"

bool ABBBGrenadeConsumable::IsUsing() const
{
    return RuntimeData.Use.ReadGrenadeUseState().bUsing;
}

bool ABBBGrenadeConsumable::IsReleased() const
{
    return RuntimeData.Use.ReadGrenadeUseState().bReleased;
}

int32 ABBBGrenadeConsumable::GetUseSerial() const
{
    return RuntimeData.Use.ReadGrenadeUseState().UseSerial;
}

const FBBBGrenadeRuntimeData &ABBBGrenadeConsumable::ReadRuntimeData() const
{
    return RuntimeData;
}

bool ABBBGrenadeConsumable::InitializeRuntimeData()
{
    return FBBBGrenadeInitializer{}.Initialize(*this);
}

void ABBBGrenadeConsumable::UpdateRuntimeData()
{
    FBBBGrenadeUpdatePipeline{}.Update(*this);
}

void ABBBGrenadeConsumable::ShutdownRuntimeData()
{
    FBBBGrenadeShutdown{}.Shutdown(*this);
}
bool ABBBGrenadeConsumable::QueueInput(FBBBConsumablePrimaryLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableEquipLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableReleaseLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableFinishLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumablePermissionLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableStateRemoteMessagePacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableStateAuthorityFactPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}
