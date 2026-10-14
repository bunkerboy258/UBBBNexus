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
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
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

bool ABBBSniperEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet)
{
    return QueueInput(FBBBSniperEquipLocalControlPacket{});
}

bool ABBBSniperEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet)
{
    return QueueInput(FBBBSniperEquipAuthorityFactPacket{});
}

bool ABBBSniperEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet)
{
    return QueueInput(FBBBSniperPrimaryLocalControlPacket{});
}

bool ABBBSniperEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket<> Packet)
{
    return QueueInput(FBBBSniperActionPermissionLocalControlPacket{{MoveTemp(Packet.Permissions)}});
}

bool ABBBSniperEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet)
{
    return QueueInput(FBBBSniperSecondaryLocalControlPacket{});
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperEquipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperEquipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperPrimaryLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperSecondaryLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperBlockFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperAllowFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperLoadAmmoLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperInterruptReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperActionPermissionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperFireRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperReloadStartRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperReloadEndRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperFireAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperReloadStartAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSniperEquipment::QueueInput(FBBBSniperReloadEndAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSniperParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSniperEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

bool ABBBSniperEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet)
{
    return QueueInput(FBBBSniperBlockFireLocalControlPacket{});
}

bool ABBBSniperEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet)
{
    return QueueInput(FBBBSniperAllowFireLocalControlPacket{});
}

bool ABBBSniperEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet)
{
    return QueueInput(FBBBSniperLoadAmmoLocalControlPacket{});
}

bool ABBBSniperEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet)
{
    return QueueInput(FBBBSniperInterruptReloadLocalControlPacket{});
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
bool ABBBSniperEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet)
{
    return false;
}

/** 本类拒绝不支持的操作 */
bool ABBBSniperEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet)
{
    return false;
}

/** 本类拒绝不支持的操作 */
bool ABBBSniperEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet)
{
    return false;
}

/** 本类拒绝不支持的操作 */
bool ABBBSniperEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet)
{
    return false;
}
