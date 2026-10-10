#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/Core/Initialization/BBBShotgunInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/Core/Update/BBBShotgunUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/Core/Shutdown/BBBShotgunShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ParseSystem/Processors/BBBShotgunParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Network/BBBShotgunNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Config/BBBShotgunDefinition.h"
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

bool ABBBShotgunEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    return QueueInput(FBBBShotgunEquipLocalControlPacket{});
}

bool ABBBShotgunEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    return QueueInput(FBBBShotgunEquipAuthorityFactPacket{});
}

bool ABBBShotgunEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    return QueueInput(FBBBShotgunFireLocalControlPacket{});
}

bool ABBBShotgunEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet)
{
    return QueueInput(FBBBShotgunActionPermissionLocalControlPacket{MoveTemp(Packet.Permissions)});
}

bool ABBBShotgunEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet)
{
    return QueueInput(FBBBShotgunReloadLocalControlPacket{});
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunEquipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunEquipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunBlockFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunAllowFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunLoadAmmoLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunInterruptReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadCycleEndLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunActionPermissionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunFireRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadStartRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadEndRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunFireAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadStartAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBShotgunEquipment::QueueInput(FBBBShotgunReloadEndAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBShotgunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBShotgunEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

bool ABBBShotgunEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet)
{
    return QueueInput(FBBBShotgunBlockFireLocalControlPacket{});
}

bool ABBBShotgunEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet)
{
    return QueueInput(FBBBShotgunAllowFireLocalControlPacket{});
}

bool ABBBShotgunEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet)
{
    return QueueInput(FBBBShotgunLoadAmmoLocalControlPacket{});
}

bool ABBBShotgunEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet)
{
    return QueueInput(FBBBShotgunInterruptReloadLocalControlPacket{});
}

bool ABBBShotgunEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}
