#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/Core/Initialization/BBBMinigunInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/Core/Update/BBBMinigunUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/Core/Shutdown/BBBMinigunShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ParseSystem/Processors/BBBMinigunParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Network/BBBMinigunNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Config/BBBMinigunDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ABBBMinigunEquipment::ABBBMinigunEquipment()
{
    NetworkComponent = CreateDefaultSubobject<UBBBMinigunNetworkComponent>(TEXT("MinigunNetwork"));
    FBBBEquipmentUpdatePipeline::ConfigureTick(*this);
}

bool ABBBMinigunEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    const UBBBMinigunDefinition *MinigunDefinition = Cast<UBBBMinigunDefinition>(GetDefinition());
    const USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    if (!MinigunDefinition || !Mesh || !Mesh->DoesSocketExist(MinigunDefinition->MuzzleSocketName))
    {
        return false;
    }

    const FTransform Muzzle = Mesh->GetSocketTransform(MinigunDefinition->MuzzleSocketName, RTS_World);
    if (!Muzzle.IsValid())
    {
        return false;
    }

    OutTransform = Muzzle;
    return true;
}

bool ABBBMinigunEquipment::InitializeRuntimeData()
{
    return FBBBMinigunInitializer{}.Initialize(*this);
}

void ABBBMinigunEquipment::ShutdownRuntimeData()
{
    FBBBMinigunShutdown{}.Shutdown(*this);
}

void ABBBMinigunEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBMinigunUpdatePipeline{}.Update(*this);
}

bool ABBBMinigunEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    return QueueInput(FBBBMinigunEquipLocalControlPacket{});
}

bool ABBBMinigunEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    return QueueInput(FBBBMinigunEquipAuthorityFactPacket{});
}

bool ABBBMinigunEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    return QueueInput(FBBBMinigunFireLocalControlPacket{});
}

bool ABBBMinigunEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet)
{
    return QueueInput(FBBBMinigunActionPermissionLocalControlPacket{MoveTemp(Packet.Permissions)});
}

bool ABBBMinigunEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet)
{
    return QueueInput(FBBBMinigunReloadLocalControlPacket{});
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunEquipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunEquipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunBlockFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunAllowFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunLoadMagazineLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunInterruptReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunActionPermissionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunFireRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunReloadStartRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunReloadEndRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunFireAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunReloadStartAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBMinigunEquipment::QueueInput(FBBBMinigunReloadEndAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBMinigunParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBMinigunEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

bool ABBBMinigunEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet)
{
    return QueueInput(FBBBMinigunBlockFireLocalControlPacket{});
}

bool ABBBMinigunEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet)
{
    return QueueInput(FBBBMinigunAllowFireLocalControlPacket{});
}

bool ABBBMinigunEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet)
{
    return QueueInput(FBBBMinigunLoadMagazineLocalControlPacket{});
}

bool ABBBMinigunEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet)
{
    return QueueInput(FBBBMinigunInterruptReloadLocalControlPacket{});
}

bool ABBBMinigunEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}
