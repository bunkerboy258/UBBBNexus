#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/Core/Initialization/BBBMeleeInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/Core/Update/BBBMeleeUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/Core/Shutdown/BBBMeleeShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ParseSystem/Processors/BBBMeleeParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Network/BBBMeleeNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentUnequipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentUnequipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndContactLocalControlPacket.h"

ABBBMeleeEquipment::ABBBMeleeEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBMeleeNetworkComponent>(TEXT("MeleeNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBMeleeEquipment::InitializeRuntimeData()
{
    return FBBBMeleeInitializer{}.Initialize(*this);
}

void ABBBMeleeEquipment::ShutdownRuntimeData()
{
    FBBBMeleeShutdown{}.Shutdown(*this);
}

void ABBBMeleeEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBMeleeUpdatePipeline{}.Update(*this);
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    return QueueInput(FBBBMeleeEquipLocalControlPacket{});
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    return QueueInput(FBBBMeleeEquipAuthorityFactPacket{});
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentUnequipLocalControlPacket Packet)
{
    return QueueInput(FBBBMeleeUnequipLocalControlPacket{});
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentUnequipAuthorityFactPacket Packet)
{
    return QueueInput(FBBBMeleeUnequipAuthorityFactPacket{});
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    return QueueInput(FBBBMeleeAttackLocalControlPacket{});
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet)
{
    return QueueInput(FBBBMeleeActionPermissionLocalControlPacket{MoveTemp(Packet.Permissions)});
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket Packet)
{
    return QueueInput(FBBBMeleeBeginActionLocalControlPacket{MoveTemp(Packet.Tokens)});
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket Packet)
{
    return QueueInput(FBBBMeleeEndActionLocalControlPacket{MoveTemp(Packet.Tokens)});
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket Packet)
{
    return QueueInput(FBBBMeleeBeginContactLocalControlPacket{MoveTemp(Packet.Tokens)});
}

bool ABBBMeleeEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket Packet)
{
    return QueueInput(FBBBMeleeEndContactLocalControlPacket{MoveTemp(Packet.Tokens)});
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeEquipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeEquipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeUnequipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeUnequipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeAttackLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeActionPermissionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeBeginActionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeBeginContactLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeEndContactLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeEndActionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeAttackStartRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeAttackEndRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeAttackStartAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMeleeEquipment::QueueInput(FBBBMeleeAttackEndAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

int32 ABBBMeleeEquipment::GetAttackSequence() const
{
    return RuntimeData.Action.ReadMeleeActionState().AttackSequence;
}

bool ABBBMeleeEquipment::IsContactOpen() const
{
    return RuntimeData.Action.ReadMeleeActionState().bContactOpen;
}

int32 ABBBMeleeEquipment::GetHitCount() const
{
    return RuntimeData.Action.ReadMeleeActionState().HitCount;
}
