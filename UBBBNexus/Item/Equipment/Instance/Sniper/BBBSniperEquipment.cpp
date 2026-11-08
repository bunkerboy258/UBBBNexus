#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/Core/Initialization/BBBSniperInitializer.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/Core/Update/BBBSniperUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/Core/Shutdown/BBBSniperShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ParseSystem/Processors/BBBSniperParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Network/BBBSniperNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Config/BBBSniperDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ABBBSniperEquipment::ABBBSniperEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBSniperNetworkComponent>(TEXT("SniperNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBSniperEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    const UBBBSniperDefinition *SniperDefinition = Cast<UBBBSniperDefinition>(GetDefinition());
    const USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    if (!SniperDefinition || !Mesh || !Mesh->DoesSocketExist(SniperDefinition->MuzzleSocketName))
    {
        return false;
    }

    const FTransform Muzzle = Mesh->GetSocketTransform(SniperDefinition->MuzzleSocketName, RTS_World);
    if (!Muzzle.IsValid())
    {
        return false;
    }

    OutTransform = Muzzle;
    return true;
}

bool ABBBSniperEquipment::InitializeRuntimeData()
{
    return FBBBSniperInitializer{}.Initialize(*this);
}

void ABBBSniperEquipment::ShutdownRuntimeData()
{
    FBBBSniperShutdown{}.Shutdown(*this);
}

void ABBBSniperEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBSniperUpdatePipeline{}.Update(*this);
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBSniperEquipLocalControlPacket{});
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBSniperEquipAuthorityFactPacket{});
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBSniperPrimaryLocalControlPacket{{Packet.PressCount}});
}



void ABBBSniperEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBSniperSecondaryLocalControlPacket{});
}

void ABBBSniperEquipment::QueueInput(FBBBSniperEquipLocalControlPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperEquipAuthorityFactPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperPrimaryLocalControlPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperSecondaryLocalControlPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperBlockFireLocalControlPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperAllowFireLocalControlPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperLoadAmmoLocalControlPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperInterruptReloadLocalControlPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}



void ABBBSniperEquipment::QueueInput(FBBBSniperFireRemoteMessagePacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperReloadStartRemoteMessagePacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperReloadEndRemoteMessagePacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperFireAuthorityFactPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperReloadStartAuthorityFactPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperReloadEndAuthorityFactPacket Packet)
{

    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBSniperBlockFireLocalControlPacket{});
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBSniperAllowFireLocalControlPacket{});
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    QueueInput(FBBBSniperLoadAmmoLocalControlPacket{});
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    QueueInput(FBBBSniperInterruptReloadLocalControlPacket{});
}

bool ABBBSniperEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}

bool ABBBSniperEquipment::CanQuickUse() const
{
    const auto &State = RuntimeData.Action.ReadSniperActionState();
    return !State.bIsReloading && !State.bFireBlocked;
}

/** 本类拒绝不支持的操作 */
void ABBBSniperEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    FBBBSniperBeginActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBSniperEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    FBBBSniperBeginContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBSniperEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    FBBBSniperEndActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBSniperEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    FBBBSniperEndContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

bool ABBBSniperEquipment::IsMeshHidden() const
{
    return RuntimeData.Action.ReadSniperActionState().bHide;
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet)
{
    FBBBSniperHideLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperHideLocalControlPacket Packet)
{
    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet)
{
    FBBBSniperShowLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperShowLocalControlPacket Packet)
{
    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet)
{
    FBBBSniperVisibilityRemoteMessagePacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperVisibilityRemoteMessagePacket Packet)
{
    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet)
{
    FBBBSniperVisibilityAuthorityFactPacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperVisibilityAuthorityFactPacket Packet)
{
    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperBeginActionLocalControlPacket Packet)
{
    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperBeginContactLocalControlPacket Packet)
{
    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperEndActionLocalControlPacket Packet)
{
    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::QueueInput(FBBBSniperEndContactLocalControlPacket Packet)
{
    FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}
