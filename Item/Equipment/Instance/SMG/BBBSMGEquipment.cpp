#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/Core/Initialization/BBBSMGInitializer.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/Core/Update/BBBSMGUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/Core/Shutdown/BBBSMGShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ParseSystem/Processors/BBBSMGParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Network/BBBSMGNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ABBBSMGEquipment::ABBBSMGEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBSMGNetworkComponent>(TEXT("SMGNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBSMGEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    const UBBBSMGDefinition *SMGDefinition = Cast<UBBBSMGDefinition>(GetDefinition());
    const USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    if (!SMGDefinition || !Mesh || !Mesh->DoesSocketExist(SMGDefinition->MuzzleSocketName))
    {
        return false;
    }

    const FTransform Muzzle = Mesh->GetSocketTransform(SMGDefinition->MuzzleSocketName, RTS_World);
    if (!Muzzle.IsValid())
    {
        return false;
    }

    OutTransform = Muzzle;
    return true;
}

bool ABBBSMGEquipment::InitializeRuntimeData()
{
    return FBBBSMGInitializer{}.Initialize(*this);
}

void ABBBSMGEquipment::ShutdownRuntimeData()
{
    FBBBSMGShutdown{}.Shutdown(*this);
}

void ABBBSMGEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBSMGUpdatePipeline{}.Update(*this);
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBSMGEquipLocalControlPacket{});
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBSMGEquipAuthorityFactPacket{});
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBSMGPrimaryLocalControlPacket{});
}



void ABBBSMGEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBSMGSecondaryLocalControlPacket{});
}

void ABBBSMGEquipment::QueueInput(FBBBSMGEquipLocalControlPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGEquipAuthorityFactPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGPrimaryLocalControlPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGSecondaryLocalControlPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGBlockFireLocalControlPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGAllowFireLocalControlPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGLoadAmmoLocalControlPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGInterruptReloadLocalControlPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}



void ABBBSMGEquipment::QueueInput(FBBBSMGFireRemoteMessagePacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGReloadStartRemoteMessagePacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGReloadEndRemoteMessagePacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGFireAuthorityFactPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGReloadStartAuthorityFactPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGReloadEndAuthorityFactPacket Packet)
{

    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBSMGBlockFireLocalControlPacket{});
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBSMGAllowFireLocalControlPacket{});
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    QueueInput(FBBBSMGLoadAmmoLocalControlPacket{});
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    QueueInput(FBBBSMGInterruptReloadLocalControlPacket{});
}

bool ABBBSMGEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}

bool ABBBSMGEquipment::CanQuickUse() const
{
    const auto &State = RuntimeData.Action.ReadSMGActionState();
    return !State.bIsReloading && !State.bFireBlocked;
}

/** 本类拒绝不支持的操作 */
void ABBBSMGEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    FBBBSMGBeginActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBSMGEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    FBBBSMGBeginContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBSMGEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    FBBBSMGEndActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBSMGEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    FBBBSMGEndContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

bool ABBBSMGEquipment::IsMeshHidden() const
{
    return RuntimeData.Action.ReadSMGActionState().bHide;
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet)
{
    FBBBSMGHideLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGHideLocalControlPacket Packet)
{
    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet)
{
    FBBBSMGShowLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGShowLocalControlPacket Packet)
{
    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet)
{
    FBBBSMGVisibilityRemoteMessagePacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGVisibilityRemoteMessagePacket Packet)
{
    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet)
{
    FBBBSMGVisibilityAuthorityFactPacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGVisibilityAuthorityFactPacket Packet)
{
    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGBeginActionLocalControlPacket Packet)
{
    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGBeginContactLocalControlPacket Packet)
{
    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGEndActionLocalControlPacket Packet)
{
    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::QueueInput(FBBBSMGEndContactLocalControlPacket Packet)
{
    FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}
