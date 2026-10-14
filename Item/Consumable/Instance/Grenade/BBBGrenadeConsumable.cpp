#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Network/BBBGrenadeNetworkComponent.h"
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
bool ABBBGrenadeConsumable::QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet)
{
    return QueueInput(FBBBGrenadePrimaryLocalControlPacket{{}});
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableEquipLocalControlPacket<> Packet)
{
    return QueueInput(FBBBGrenadeEquipLocalControlPacket{{}});
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableReleaseLocalControlPacket<> Packet)
{
    return QueueInput(FBBBGrenadeReleaseLocalControlPacket{{}});
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet)
{
    return QueueInput(FBBBGrenadeFinishLocalControlPacket{{}});
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumablePermissionLocalControlPacket<> Packet)
{
    return QueueInput(FBBBGrenadePermissionLocalControlPacket{{MoveTemp(Packet.Values)}});
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableStateRemoteMessagePacket<> Packet)
{
    return false;
}

bool ABBBGrenadeConsumable::QueueInput(FBBBConsumableStateAuthorityFactPacket<> Packet)
{
    return false;
}

bool ABBBGrenadeConsumable::QueueInput(FBBBGrenadeStateRemoteMessagePacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBGrenadeEquipLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBGrenadeFinishLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBGrenadePermissionLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBGrenadePrimaryLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBGrenadeReleaseLocalControlPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::QueueInput(FBBBGrenadeStateAuthorityFactPacket Packet)
{
    return ParseSystem.Submit(MoveTemp(Packet));
}

ABBBGrenadeConsumable::ABBBGrenadeConsumable()
{
    NetworkComponent = CreateDefaultSubobject<UBBBGrenadeNetworkComponent>(TEXT("GrenadeProtocol"));
}
