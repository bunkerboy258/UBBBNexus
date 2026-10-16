#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/Core/Initialization/BBBRifleInitializer.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/Core/Shutdown/BBBRifleShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Network/BBBRifleNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ABBBRifleEquipment::ABBBRifleEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBRifleNetworkComponent>(TEXT("RifleNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBRifleEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(GetDefinition());
    const USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    if (!RifleDefinition || !Mesh || !Mesh->DoesSocketExist(RifleDefinition->MuzzleSocketName))
    {
        return false;
    }

    const FTransform Muzzle = Mesh->GetSocketTransform(RifleDefinition->MuzzleSocketName, RTS_World);
    if (!Muzzle.IsValid())
    {
        return false;
    }

    OutTransform = Muzzle;
    return true;
}

bool ABBBRifleEquipment::InitializeRuntimeData()
{
    return FBBBRifleInitializer{}.Initialize(*this);
}

void ABBBRifleEquipment::ShutdownRuntimeData()
{
    FBBBRifleShutdown{}.Shutdown(*this);
}

void ABBBRifleEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBRifleUpdatePipeline{}.Update(*this);
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBRifleEquipLocalControlPacket{});
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBRifleEquipAuthorityFactPacket{});
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBRiflePrimaryLocalControlPacket{});
}



void ABBBRifleEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBRifleSecondaryLocalControlPacket{});
}

void ABBBRifleEquipment::QueueInput(FBBBRifleEquipLocalControlPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleEquipAuthorityFactPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRiflePrimaryLocalControlPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleSecondaryLocalControlPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleBlockFireLocalControlPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleAllowFireLocalControlPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleLoadAmmoLocalControlPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleInterruptReloadLocalControlPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}



void ABBBRifleEquipment::QueueInput(FBBBRifleFireRemoteMessagePacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleReloadStartRemoteMessagePacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleReloadEndRemoteMessagePacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleFireAuthorityFactPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleReloadStartAuthorityFactPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleReloadEndAuthorityFactPacket Packet)
{

    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBRifleBlockFireLocalControlPacket{});
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBRifleAllowFireLocalControlPacket{});
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    QueueInput(FBBBRifleLoadAmmoLocalControlPacket{});
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    QueueInput(FBBBRifleInterruptReloadLocalControlPacket{});
}

bool ABBBRifleEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}

bool ABBBRifleEquipment::CanQuickUse() const
{
    const auto &State = RuntimeData.Action.ReadRifleActionState();
    return !State.bIsReloading && !State.bFireBlocked;
}

/** 本类拒绝不支持的操作 */
void ABBBRifleEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    FBBBRifleBeginActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBRifleEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    FBBBRifleBeginContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBRifleEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    FBBBRifleEndActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBRifleEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    FBBBRifleEndContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

bool ABBBRifleEquipment::IsMeshHidden() const
{
    return RuntimeData.Action.ReadRifleActionState().bHide;
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet)
{
    FBBBRifleHideLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleHideLocalControlPacket Packet)
{
    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet)
{
    FBBBRifleShowLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleShowLocalControlPacket Packet)
{
    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet)
{
    FBBBRifleVisibilityRemoteMessagePacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleVisibilityRemoteMessagePacket Packet)
{
    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet)
{
    FBBBRifleVisibilityAuthorityFactPacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleVisibilityAuthorityFactPacket Packet)
{
    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleBeginActionLocalControlPacket Packet)
{
    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleBeginContactLocalControlPacket Packet)
{
    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleEndActionLocalControlPacket Packet)
{
    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::QueueInput(FBBBRifleEndContactLocalControlPacket Packet)
{
    FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}
