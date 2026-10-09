#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/Core/Initialization/BBBRevolverInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/Core/Update/BBBRevolverUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/Core/Shutdown/BBBRevolverShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ParseSystem/Processors/BBBRevolverParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Network/BBBRevolverNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
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

bool ABBBRevolverEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    return QueueInput(FBBBRevolverEquipLocalControlPacket{});
}

bool ABBBRevolverEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    return QueueInput(FBBBRevolverEquipAuthorityFactPacket{});
}

bool ABBBRevolverEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    return QueueInput(FBBBRevolverFireLocalControlPacket{});
}

bool ABBBRevolverEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet)
{
    return QueueInput(FBBBRevolverActionPermissionLocalControlPacket{MoveTemp(Packet.Permissions)});
}

bool ABBBRevolverEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet)
{
    return QueueInput(FBBBRevolverReloadLocalControlPacket{});
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverEquipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverEquipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverBlockFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverAllowFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverLoadMagazineLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverInterruptReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverActionPermissionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverFireRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverReloadStartRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverReloadEndRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverFireAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverReloadStartAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRevolverEquipment::QueueInput(FBBBRevolverReloadEndAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRevolverParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRevolverEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

bool ABBBRevolverEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet)
{
    return QueueInput(FBBBRevolverBlockFireLocalControlPacket{});
}

bool ABBBRevolverEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet)
{
    return QueueInput(FBBBRevolverAllowFireLocalControlPacket{});
}

bool ABBBRevolverEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet)
{
    return QueueInput(FBBBRevolverLoadMagazineLocalControlPacket{});
}

bool ABBBRevolverEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet)
{
    return QueueInput(FBBBRevolverInterruptReloadLocalControlPacket{});
}

bool ABBBRevolverEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}
