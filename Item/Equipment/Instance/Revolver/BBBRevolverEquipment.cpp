#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/Core/Initialization/BBBRevolverInitializer.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/Core/Update/BBBRevolverUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/Core/Shutdown/BBBRevolverShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ParseSystem/Processors/BBBRevolverParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Network/BBBRevolverNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ABBBRevolverEquipment::ABBBRevolverEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBRevolverNetworkComponent>(TEXT("RevolverNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBRevolverEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    const UBBBRevolverDefinition *RevolverDefinition = Cast<UBBBRevolverDefinition>(GetDefinition());
    const USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    if (!RevolverDefinition || !Mesh || !Mesh->DoesSocketExist(RevolverDefinition->MuzzleSocketName))
    {
        return false;
    }

    const FTransform Muzzle = Mesh->GetSocketTransform(RevolverDefinition->MuzzleSocketName, RTS_World);
    if (!Muzzle.IsValid())
    {
        return false;
    }

    OutTransform = Muzzle;
    return true;
}

bool ABBBRevolverEquipment::InitializeRuntimeData()
{
    return FBBBRevolverInitializer{}.Initialize(*this);
}

void ABBBRevolverEquipment::ShutdownRuntimeData()
{
    FBBBRevolverShutdown{}.Shutdown(*this);
}

void ABBBRevolverEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBRevolverUpdatePipeline{}.Update(*this);
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBRevolverEquipLocalControlPacket{});
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBRevolverEquipAuthorityFactPacket{});
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBRevolverPrimaryLocalControlPacket{});
}



void ABBBRevolverEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBRevolverSecondaryLocalControlPacket{});
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverEquipLocalControlPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverEquipAuthorityFactPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverPrimaryLocalControlPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverSecondaryLocalControlPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverBlockFireLocalControlPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverAllowFireLocalControlPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverLoadAmmoLocalControlPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverInterruptReloadLocalControlPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}



void ABBBRevolverEquipment::QueueInput(FBBBRevolverFireRemoteMessagePacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverReloadStartRemoteMessagePacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverReloadEndRemoteMessagePacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverFireAuthorityFactPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverReloadStartAuthorityFactPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverReloadEndAuthorityFactPacket Packet)
{

    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBRevolverBlockFireLocalControlPacket{});
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBRevolverAllowFireLocalControlPacket{});
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    QueueInput(FBBBRevolverLoadAmmoLocalControlPacket{});
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    QueueInput(FBBBRevolverInterruptReloadLocalControlPacket{});
}

bool ABBBRevolverEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}

bool ABBBRevolverEquipment::CanQuickUse() const
{
    const auto &State = RuntimeData.Action.ReadRevolverActionState();
    return !State.bIsReloading && !State.bFireBlocked;
}

/** 本类拒绝不支持的操作 */
void ABBBRevolverEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    FBBBRevolverBeginActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBRevolverEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    FBBBRevolverBeginContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBRevolverEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    FBBBRevolverEndActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBRevolverEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    FBBBRevolverEndContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

bool ABBBRevolverEquipment::IsMeshHidden() const
{
    return RuntimeData.Action.ReadRevolverActionState().bHide;
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet)
{
    FBBBRevolverHideLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverHideLocalControlPacket Packet)
{
    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet)
{
    FBBBRevolverShowLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverShowLocalControlPacket Packet)
{
    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet)
{
    FBBBRevolverVisibilityRemoteMessagePacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverVisibilityRemoteMessagePacket Packet)
{
    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet)
{
    FBBBRevolverVisibilityAuthorityFactPacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverVisibilityAuthorityFactPacket Packet)
{
    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverBeginActionLocalControlPacket Packet)
{
    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverBeginContactLocalControlPacket Packet)
{
    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverEndActionLocalControlPacket Packet)
{
    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::QueueInput(FBBBRevolverEndContactLocalControlPacket Packet)
{
    FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}
