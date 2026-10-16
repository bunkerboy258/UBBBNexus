#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/Core/Initialization/BBBShotgunInitializer.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/Core/Update/BBBShotgunUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/Core/Shutdown/BBBShotgunShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ParseSystem/Processors/BBBShotgunParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Network/BBBShotgunNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Config/BBBShotgunDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ABBBShotgunEquipment::ABBBShotgunEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBShotgunNetworkComponent>(TEXT("ShotgunNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBShotgunEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    const UBBBShotgunDefinition *ShotgunDefinition = Cast<UBBBShotgunDefinition>(GetDefinition());
    const USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    if (!ShotgunDefinition || !Mesh || !Mesh->DoesSocketExist(ShotgunDefinition->MuzzleSocketName))
    {
        return false;
    }

    const FTransform Muzzle = Mesh->GetSocketTransform(ShotgunDefinition->MuzzleSocketName, RTS_World);
    if (!Muzzle.IsValid())
    {
        return false;
    }

    OutTransform = Muzzle;
    return true;
}

bool ABBBShotgunEquipment::InitializeRuntimeData()
{
    return FBBBShotgunInitializer{}.Initialize(*this);
}

void ABBBShotgunEquipment::ShutdownRuntimeData()
{
    FBBBShotgunShutdown{}.Shutdown(*this);
}

void ABBBShotgunEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBShotgunUpdatePipeline{}.Update(*this);
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBShotgunEquipLocalControlPacket{});
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBShotgunEquipAuthorityFactPacket{});
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBShotgunPrimaryLocalControlPacket{});
}



void ABBBShotgunEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBShotgunSecondaryLocalControlPacket{});
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunEquipLocalControlPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunEquipAuthorityFactPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunPrimaryLocalControlPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunSecondaryLocalControlPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunBlockFireLocalControlPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunAllowFireLocalControlPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunLoadAmmoLocalControlPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunInterruptReloadLocalControlPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadCycleEndLocalControlPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}



void ABBBShotgunEquipment::QueueInput(FBBBShotgunFireRemoteMessagePacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadStartRemoteMessagePacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadEndRemoteMessagePacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunFireAuthorityFactPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadStartAuthorityFactPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadEndAuthorityFactPacket Packet)
{

    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBShotgunBlockFireLocalControlPacket{});
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBShotgunAllowFireLocalControlPacket{});
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    QueueInput(FBBBShotgunLoadAmmoLocalControlPacket{});
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    QueueInput(FBBBShotgunInterruptReloadLocalControlPacket{});
}

bool ABBBShotgunEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}

bool ABBBShotgunEquipment::CanQuickUse() const
{
    const auto &State = RuntimeData.Action.ReadShotgunActionState();
    return !State.bIsReloading && !State.bFireBlocked;
}

/** 本类拒绝不支持的操作 */
void ABBBShotgunEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    FBBBShotgunBeginActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBShotgunEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    FBBBShotgunBeginContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBShotgunEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    FBBBShotgunEndActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBShotgunEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    FBBBShotgunEndContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

bool ABBBShotgunEquipment::IsMeshHidden() const
{
    return RuntimeData.Action.ReadShotgunActionState().bHide;
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet)
{
    FBBBShotgunHideLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunHideLocalControlPacket Packet)
{
    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet)
{
    FBBBShotgunShowLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunShowLocalControlPacket Packet)
{
    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet)
{
    FBBBShotgunVisibilityRemoteMessagePacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunVisibilityRemoteMessagePacket Packet)
{
    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet)
{
    FBBBShotgunVisibilityAuthorityFactPacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunVisibilityAuthorityFactPacket Packet)
{
    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunBeginActionLocalControlPacket Packet)
{
    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunBeginContactLocalControlPacket Packet)
{
    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunEndActionLocalControlPacket Packet)
{
    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::QueueInput(FBBBShotgunEndContactLocalControlPacket Packet)
{
    FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}
