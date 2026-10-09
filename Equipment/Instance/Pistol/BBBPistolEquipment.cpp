#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/Core/Initialization/BBBPistolInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/Core/Update/BBBPistolUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/Core/Shutdown/BBBPistolShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ParseSystem/Processors/BBBPistolParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Network/BBBPistolNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
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

bool ABBBPistolEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    return QueueInput(FBBBPistolEquipLocalControlPacket{});
}

bool ABBBPistolEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    return QueueInput(FBBBPistolEquipAuthorityFactPacket{});
}

bool ABBBPistolEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    return QueueInput(FBBBPistolFireLocalControlPacket{});
}

bool ABBBPistolEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet)
{
    return QueueInput(FBBBPistolActionPermissionLocalControlPacket{MoveTemp(Packet.Permissions)});
}

bool ABBBPistolEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet)
{
    return QueueInput(FBBBPistolReloadLocalControlPacket{});
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolEquipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolEquipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolBlockFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolAllowFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolLoadMagazineLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolInterruptReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolActionPermissionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolFireRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolReloadStartRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolReloadEndRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolFireAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolReloadStartAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBPistolEquipment::QueueInput(FBBBPistolReloadEndAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBPistolParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBPistolEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

bool ABBBPistolEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet)
{
    return QueueInput(FBBBPistolBlockFireLocalControlPacket{});
}

bool ABBBPistolEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet)
{
    return QueueInput(FBBBPistolAllowFireLocalControlPacket{});
}

bool ABBBPistolEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet)
{
    return QueueInput(FBBBPistolLoadMagazineLocalControlPacket{});
}

bool ABBBPistolEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet)
{
    return QueueInput(FBBBPistolInterruptReloadLocalControlPacket{});
}

bool ABBBPistolEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}
