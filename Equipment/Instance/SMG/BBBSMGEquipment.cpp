#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/Core/Initialization/BBBSMGInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/Core/Update/BBBSMGUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/Core/Shutdown/BBBSMGShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ParseSystem/Processors/BBBSMGParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Network/BBBSMGNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
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

bool ABBBSMGEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    return QueueInput(FBBBSMGEquipLocalControlPacket{});
}

bool ABBBSMGEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    return QueueInput(FBBBSMGEquipAuthorityFactPacket{});
}

bool ABBBSMGEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    return QueueInput(FBBBSMGFireLocalControlPacket{});
}

bool ABBBSMGEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet)
{
    return QueueInput(FBBBSMGActionPermissionLocalControlPacket{MoveTemp(Packet.Permissions)});
}

bool ABBBSMGEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet)
{
    return QueueInput(FBBBSMGReloadLocalControlPacket{});
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGEquipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGEquipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGBlockFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGAllowFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGLoadMagazineLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGInterruptReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGActionPermissionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGFireRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGReloadStartRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGReloadEndRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGFireAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGReloadStartAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBSMGEquipment::QueueInput(FBBBSMGReloadEndAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBSMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBSMGEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

bool ABBBSMGEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet)
{
    return QueueInput(FBBBSMGBlockFireLocalControlPacket{});
}

bool ABBBSMGEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet)
{
    return QueueInput(FBBBSMGAllowFireLocalControlPacket{});
}

bool ABBBSMGEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet)
{
    return QueueInput(FBBBSMGLoadMagazineLocalControlPacket{});
}

bool ABBBSMGEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet)
{
    return QueueInput(FBBBSMGInterruptReloadLocalControlPacket{});
}

bool ABBBSMGEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}
