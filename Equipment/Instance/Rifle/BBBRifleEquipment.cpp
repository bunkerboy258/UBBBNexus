#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Initialization/BBBRifleInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Shutdown/BBBRifleShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Network/BBBRifleNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
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

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    return QueueInput(FBBBRifleEquipLocalControlPacket{});
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    return QueueInput(FBBBRifleEquipAuthorityFactPacket{});
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    return QueueInput(FBBBRifleFireLocalControlPacket{});
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet)
{
    return QueueInput(FBBBRifleActionPermissionLocalControlPacket{MoveTemp(Packet.Permissions)});
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet)
{
    return QueueInput(FBBBRifleReloadLocalControlPacket{});
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleEquipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleEquipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleBlockFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleAllowFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleLoadMagazineLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleInterruptReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleActionPermissionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleFireRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleReloadStartRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleReloadEndRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleFireAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleReloadStartAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleReloadEndAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBRifleParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBRifleEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet)
{
    return QueueInput(FBBBRifleBlockFireLocalControlPacket{});
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet)
{
    return QueueInput(FBBBRifleAllowFireLocalControlPacket{});
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet)
{
    return QueueInput(FBBBRifleLoadMagazineLocalControlPacket{});
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet)
{
    return QueueInput(FBBBRifleInterruptReloadLocalControlPacket{});
}

bool ABBBRifleEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}
