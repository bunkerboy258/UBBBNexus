#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/Core/Initialization/BBBPistolInitializer.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/Core/Update/BBBPistolUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/Core/Shutdown/BBBPistolShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ParseSystem/Processors/BBBPistolParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Network/BBBPistolNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ABBBPistolEquipment::ABBBPistolEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBPistolNetworkComponent>(TEXT("PistolNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBPistolEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    const UBBBPistolDefinition *PistolDefinition = Cast<UBBBPistolDefinition>(GetDefinition());
    const USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    if (!PistolDefinition || !Mesh || !Mesh->DoesSocketExist(PistolDefinition->MuzzleSocketName))
    {
        return false;
    }

    const FTransform Muzzle = Mesh->GetSocketTransform(PistolDefinition->MuzzleSocketName, RTS_World);
    if (!Muzzle.IsValid())
    {
        return false;
    }

    OutTransform = Muzzle;
    return true;
}

bool ABBBPistolEquipment::InitializeRuntimeData()
{
    return FBBBPistolInitializer{}.Initialize(*this);
}

void ABBBPistolEquipment::ShutdownRuntimeData()
{
    FBBBPistolShutdown{}.Shutdown(*this);
}

void ABBBPistolEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBPistolUpdatePipeline{}.Update(*this);
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBPistolEquipLocalControlPacket{});
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBPistolEquipAuthorityFactPacket{});
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBPistolPrimaryLocalControlPacket{{Packet.PressCount}});
}



void ABBBPistolEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBPistolSecondaryLocalControlPacket{});
}

void ABBBPistolEquipment::QueueInput(FBBBPistolEquipLocalControlPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolEquipAuthorityFactPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolPrimaryLocalControlPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolSecondaryLocalControlPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolBlockFireLocalControlPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolAllowFireLocalControlPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolLoadAmmoLocalControlPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolInterruptReloadLocalControlPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}



void ABBBPistolEquipment::QueueInput(FBBBPistolFireRemoteMessagePacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolReloadStartRemoteMessagePacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolReloadEndRemoteMessagePacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolFireAuthorityFactPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolReloadStartAuthorityFactPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolReloadEndAuthorityFactPacket Packet)
{

    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBPistolBlockFireLocalControlPacket{});
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBPistolAllowFireLocalControlPacket{});
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    QueueInput(FBBBPistolLoadAmmoLocalControlPacket{});
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    QueueInput(FBBBPistolInterruptReloadLocalControlPacket{});
}

bool ABBBPistolEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}

bool ABBBPistolEquipment::CanQuickUse() const
{
    const auto &State = RuntimeData.Action.ReadPistolActionState();
    return !State.bIsReloading && !State.bFireBlocked;
}

/** 本类拒绝不支持的操作 */
void ABBBPistolEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    FBBBPistolBeginActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBPistolEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    FBBBPistolBeginContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBPistolEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    FBBBPistolEndActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBPistolEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    FBBBPistolEndContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

bool ABBBPistolEquipment::IsMeshHidden() const
{
    return RuntimeData.Action.ReadPistolActionState().bHide;
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet)
{
    FBBBPistolHideLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolHideLocalControlPacket Packet)
{
    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet)
{
    FBBBPistolShowLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolShowLocalControlPacket Packet)
{
    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet)
{
    FBBBPistolVisibilityRemoteMessagePacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolVisibilityRemoteMessagePacket Packet)
{
    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet)
{
    FBBBPistolVisibilityAuthorityFactPacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolVisibilityAuthorityFactPacket Packet)
{
    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolBeginActionLocalControlPacket Packet)
{
    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolBeginContactLocalControlPacket Packet)
{
    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolEndActionLocalControlPacket Packet)
{
    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::QueueInput(FBBBPistolEndContactLocalControlPacket Packet)
{
    FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}
