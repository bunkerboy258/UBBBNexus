#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/Core/Initialization/BBBLMGInitializer.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/Core/Update/BBBLMGUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/Core/Shutdown/BBBLMGShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ParseSystem/Processors/BBBLMGParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Network/BBBLMGNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Config/BBBLMGDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ABBBLMGEquipment::ABBBLMGEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBLMGNetworkComponent>(TEXT("LMGNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBLMGEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    const UBBBLMGDefinition *LMGDefinition = Cast<UBBBLMGDefinition>(GetDefinition());
    const USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    if (!LMGDefinition || !Mesh || !Mesh->DoesSocketExist(LMGDefinition->MuzzleSocketName))
    {
        return false;
    }

    const FTransform Muzzle = Mesh->GetSocketTransform(LMGDefinition->MuzzleSocketName, RTS_World);
    if (!Muzzle.IsValid())
    {
        return false;
    }

    OutTransform = Muzzle;
    return true;
}

bool ABBBLMGEquipment::InitializeRuntimeData()
{
    return FBBBLMGInitializer{}.Initialize(*this);
}

void ABBBLMGEquipment::ShutdownRuntimeData()
{
    FBBBLMGShutdown{}.Shutdown(*this);
}

void ABBBLMGEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBLMGUpdatePipeline{}.Update(*this);
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    QueueInput(FBBBLMGEquipLocalControlPacket{});
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    QueueInput(FBBBLMGEquipAuthorityFactPacket{});
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBLMGPrimaryLocalControlPacket{{Packet.PressCount}});
}



void ABBBLMGEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    QueueInput(FBBBLMGSecondaryLocalControlPacket{});
}

void ABBBLMGEquipment::QueueInput(FBBBLMGEquipLocalControlPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGEquipAuthorityFactPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGPrimaryLocalControlPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGSecondaryLocalControlPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGBlockFireLocalControlPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGAllowFireLocalControlPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGLoadAmmoLocalControlPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGInterruptReloadLocalControlPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}



void ABBBLMGEquipment::QueueInput(FBBBLMGFireRemoteMessagePacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGReloadStartRemoteMessagePacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGReloadEndRemoteMessagePacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGFireAuthorityFactPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGReloadStartAuthorityFactPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGReloadEndAuthorityFactPacket Packet)
{

    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBLMGBlockFireLocalControlPacket{});
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    QueueInput(FBBBLMGAllowFireLocalControlPacket{});
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    QueueInput(FBBBLMGLoadAmmoLocalControlPacket{});
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    QueueInput(FBBBLMGInterruptReloadLocalControlPacket{});
}

bool ABBBLMGEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}

bool ABBBLMGEquipment::CanQuickUse() const
{
    const auto &State = RuntimeData.Action.ReadLMGActionState();
    return !State.bIsReloading && !State.bFireBlocked;
}

/** 本类拒绝不支持的操作 */
void ABBBLMGEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    FBBBLMGBeginActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBLMGEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    FBBBLMGBeginContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBLMGEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    FBBBLMGEndActionLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

/** 本类拒绝不支持的操作 */
void ABBBLMGEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    FBBBLMGEndContactLocalControlPacket Concrete;
    Concrete.Tokens = MoveTemp(Packet.Tokens);
    QueueInput(MoveTemp(Concrete));
}

bool ABBBLMGEquipment::IsMeshHidden() const
{
    return RuntimeData.Action.ReadLMGActionState().bHide;
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet)
{
    FBBBLMGHideLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGHideLocalControlPacket Packet)
{
    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet)
{
    FBBBLMGShowLocalControlPacket Concrete;
    QueueInput(MoveTemp(Concrete));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGShowLocalControlPacket Packet)
{
    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet)
{
    FBBBLMGVisibilityRemoteMessagePacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGVisibilityRemoteMessagePacket Packet)
{
    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet)
{
    FBBBLMGVisibilityAuthorityFactPacket Concrete;
    Concrete.Hidden = MoveTemp(Packet.Hidden);
    Concrete.Revisions = MoveTemp(Packet.Revisions);
    QueueInput(MoveTemp(Concrete));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGVisibilityAuthorityFactPacket Packet)
{
    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGBeginActionLocalControlPacket Packet)
{
    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGBeginContactLocalControlPacket Packet)
{
    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGEndActionLocalControlPacket Packet)
{
    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::QueueInput(FBBBLMGEndContactLocalControlPacket Packet)
{
    FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}
