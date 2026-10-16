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
void ABBBGrenadeConsumable::QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBGrenadePrimaryLocalControlPacket{{}});
}

void ABBBGrenadeConsumable::QueueInput(FBBBConsumableEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBGrenadeEquipLocalControlPacket{{}});
}

void ABBBGrenadeConsumable::QueueInput(FBBBConsumableReleaseLocalControlPacket<> Packet)
{
    QueueInput(FBBBGrenadeReleaseLocalControlPacket{{}});
}

void ABBBGrenadeConsumable::QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet)
{
    QueueInput(FBBBGrenadeFinishLocalControlPacket{{}});
}



void ABBBGrenadeConsumable::QueueInput(FBBBConsumableStateRemoteMessagePacket<> Packet)
{
    QueueInput(FBBBGrenadeStateRemoteMessagePacket{});
}

void ABBBGrenadeConsumable::QueueInput(FBBBConsumableStateAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBGrenadeStateAuthorityFactPacket{});
}

void ABBBGrenadeConsumable::QueueInput(FBBBGrenadeStateRemoteMessagePacket Packet)
{
    ParseSystem.Submit(MoveTemp(Packet));
}

void ABBBGrenadeConsumable::QueueInput(FBBBGrenadeEquipLocalControlPacket Packet)
{
    ParseSystem.Submit(MoveTemp(Packet));
}

void ABBBGrenadeConsumable::QueueInput(FBBBGrenadeFinishLocalControlPacket Packet)
{
    ParseSystem.Submit(MoveTemp(Packet));
}



void ABBBGrenadeConsumable::QueueInput(FBBBGrenadePrimaryLocalControlPacket Packet)
{
    ParseSystem.Submit(MoveTemp(Packet));
}

void ABBBGrenadeConsumable::QueueInput(FBBBGrenadeReleaseLocalControlPacket Packet)
{
    ParseSystem.Submit(MoveTemp(Packet));
}

void ABBBGrenadeConsumable::QueueInput(FBBBGrenadeStateAuthorityFactPacket Packet)
{
    ParseSystem.Submit(MoveTemp(Packet));
}

ABBBGrenadeConsumable::ABBBGrenadeConsumable()
{
    NetworkComponent = CreateDefaultSubobject<UBBBGrenadeNetworkComponent>(TEXT("GrenadeProtocol"));
}

void ABBBGrenadeConsumable::QueueInput(FBBBConsumableHideLocalControlPacket<> Packet)
{
    QueueInput(FBBBGrenadeHideLocalControlPacket{{}});
}
void ABBBGrenadeConsumable::QueueInput(FBBBGrenadeHideLocalControlPacket Packet)
{
    ParseSystem.Submit(MoveTemp(Packet));
}

void ABBBGrenadeConsumable::QueueInput(FBBBConsumableShowLocalControlPacket<> Packet)
{
    QueueInput(FBBBGrenadeShowLocalControlPacket{{}});
}
void ABBBGrenadeConsumable::QueueInput(FBBBGrenadeShowLocalControlPacket Packet)
{
    ParseSystem.Submit(MoveTemp(Packet));
}

bool ABBBGrenadeConsumable::IsMeshHidden() const
{
    return RuntimeData.Use.ReadGrenadeUseState().bHide;
}
