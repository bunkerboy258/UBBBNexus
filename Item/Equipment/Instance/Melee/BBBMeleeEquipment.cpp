#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/Core/Initialization/BBBMeleeInitializer.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/Core/Update/BBBMeleeUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/Core/Shutdown/BBBMeleeShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ParseSystem/Processors/BBBMeleeParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Network/BBBMeleeNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndContactLocalControlPacket.h"

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

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBMeleeEquipLocalControlPacket{});
}

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBMeleeEquipAuthorityFactPacket{});
}

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBMeleePrimaryLocalControlPacket{{Packet.PressCount}});
}



void ABBBMeleeEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    QueueInput(FBBBMeleeBeginActionLocalControlPacket{{MoveTemp(Packet.Tokens)}});
}

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    QueueInput(FBBBMeleeEndActionLocalControlPacket{{MoveTemp(Packet.Tokens)}});
}

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    QueueInput(FBBBMeleeBeginContactLocalControlPacket{{MoveTemp(Packet.Tokens)}});
}

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    QueueInput(FBBBMeleeEndContactLocalControlPacket{{MoveTemp(Packet.Tokens)}});
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeEquipLocalControlPacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeEquipAuthorityFactPacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleePrimaryLocalControlPacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}



void ABBBMeleeEquipment::QueueInput(FBBBMeleeBeginActionLocalControlPacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeBeginContactLocalControlPacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeEndContactLocalControlPacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeEndActionLocalControlPacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeAttackStartRemoteMessagePacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeAttackEndRemoteMessagePacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeAttackStartAuthorityFactPacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeAttackEndAuthorityFactPacket Packet)
{

    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
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

bool ABBBMeleeEquipment::CanSwitch() const
{
    return !RuntimeData.Action.ReadMeleeActionState().bAttacking;
}

/** 本类拒绝不支持的操作 */
void ABBBMeleeEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    FBBBMeleeSecondaryLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBMeleeEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    FBBBMeleeAllowFireLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBMeleeEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    FBBBMeleeBlockFireLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBMeleeEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    FBBBMeleeInterruptReloadLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBMeleeEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    FBBBMeleeLoadAmmoLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

bool ABBBMeleeEquipment::IsMeshHidden() const
{
    return RuntimeData.Action.ReadMeleeActionState().bHide;
}

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet)
{
    FBBBMeleeHideLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeHideLocalControlPacket Packet)
{
    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet)
{
    FBBBMeleeShowLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeShowLocalControlPacket Packet)
{
    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet)
{
    FBBBMeleeVisibilityRemoteMessagePacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeVisibilityRemoteMessagePacket Packet)
{
    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet)
{
    FBBBMeleeVisibilityAuthorityFactPacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeVisibilityAuthorityFactPacket Packet)
{
    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeSecondaryLocalControlPacket Packet)
{
    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeAllowFireLocalControlPacket Packet)
{
    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeBlockFireLocalControlPacket Packet)
{
    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeInterruptReloadLocalControlPacket Packet)
{
    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMeleeEquipment::QueueInput(FBBBMeleeLoadAmmoLocalControlPacket Packet)
{
    FBBBMeleeParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}
