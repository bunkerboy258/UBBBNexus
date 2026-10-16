#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/Core/Initialization/BBBMinigunInitializer.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/Core/Update/BBBMinigunUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/Core/Shutdown/BBBMinigunShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ParseSystem/Processors/BBBMinigunParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Network/BBBMinigunNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Config/BBBMinigunDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ABBBMinigunEquipment::ABBBMinigunEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBMinigunNetworkComponent>(TEXT("MinigunNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBMinigunEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    const UBBBMinigunDefinition *MinigunDefinition = Cast<UBBBMinigunDefinition>(GetDefinition());
    const USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    if (!MinigunDefinition || !Mesh || !Mesh->DoesSocketExist(MinigunDefinition->MuzzleSocketName))
    {
        return false;
    }

    const FTransform Muzzle = Mesh->GetSocketTransform(MinigunDefinition->MuzzleSocketName, RTS_World);
    if (!Muzzle.IsValid())
    {
        return false;
    }

    OutTransform = Muzzle;
    return true;
}

bool ABBBMinigunEquipment::InitializeRuntimeData()
{
    return FBBBMinigunInitializer{}.Initialize(*this);
}

void ABBBMinigunEquipment::ShutdownRuntimeData()
{
    FBBBMinigunShutdown{}.Shutdown(*this);
}

void ABBBMinigunEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBMinigunUpdatePipeline{}.Update(*this);
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBMinigunEquipLocalControlPacket{});
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBMinigunEquipAuthorityFactPacket{});
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBMinigunPrimaryLocalControlPacket{});
}



void ABBBMinigunEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBMinigunSecondaryLocalControlPacket{});
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunEquipLocalControlPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunEquipAuthorityFactPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunPrimaryLocalControlPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunSecondaryLocalControlPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunBlockFireLocalControlPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunAllowFireLocalControlPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunLoadAmmoLocalControlPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunInterruptReloadLocalControlPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}



void ABBBMinigunEquipment::QueueInput(FBBBMinigunFireRemoteMessagePacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunSpinRemoteMessagePacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunSpinAuthorityFactPacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunReloadStartRemoteMessagePacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunReloadEndRemoteMessagePacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunFireAuthorityFactPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunReloadStartAuthorityFactPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunReloadEndAuthorityFactPacket Packet)
{

    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBMinigunBlockFireLocalControlPacket{});
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBMinigunAllowFireLocalControlPacket{});
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    QueueInput(FBBBMinigunLoadAmmoLocalControlPacket{});
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    QueueInput(FBBBMinigunInterruptReloadLocalControlPacket{});
}

bool ABBBMinigunEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}

bool ABBBMinigunEquipment::CanQuickUse() const
{
    const auto &State = RuntimeData.Action.ReadMinigunActionState();
    return !State.bIsReloading && !State.bFireBlocked;
}

/** 本类拒绝不支持的操作 */
void ABBBMinigunEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    FBBBMinigunBeginActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBMinigunEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    FBBBMinigunBeginContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBMinigunEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    FBBBMinigunEndActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBMinigunEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    FBBBMinigunEndContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

bool ABBBMinigunEquipment::IsMeshHidden() const
{
    return RuntimeData.Action.ReadMinigunActionState().bHide;
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet)
{
    FBBBMinigunHideLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunHideLocalControlPacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet)
{
    FBBBMinigunShowLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunShowLocalControlPacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet)
{
    FBBBMinigunVisibilityRemoteMessagePacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunVisibilityRemoteMessagePacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet)
{
    FBBBMinigunVisibilityAuthorityFactPacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunVisibilityAuthorityFactPacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunBeginActionLocalControlPacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunBeginContactLocalControlPacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunEndActionLocalControlPacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::QueueInput(FBBBMinigunEndContactLocalControlPacket Packet)
{
    FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}
