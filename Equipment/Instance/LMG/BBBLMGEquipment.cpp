#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/Core/Initialization/BBBLMGInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/Core/Update/BBBLMGUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/Core/Shutdown/BBBLMGShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ParseSystem/Processors/BBBLMGParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Network/BBBLMGNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Config/BBBLMGDefinition.h"
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

bool ABBBLMGEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    return QueueInput(FBBBLMGEquipLocalControlPacket{});
}

bool ABBBLMGEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    return QueueInput(FBBBLMGEquipAuthorityFactPacket{});
}

bool ABBBLMGEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    return QueueInput(FBBBLMGFireLocalControlPacket{});
}

bool ABBBLMGEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet)
{
    return QueueInput(FBBBLMGActionPermissionLocalControlPacket{MoveTemp(Packet.Permissions)});
}

bool ABBBLMGEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet)
{
    return QueueInput(FBBBLMGReloadLocalControlPacket{});
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGEquipLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGEquipAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGBlockFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGAllowFireLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGLoadMagazineLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGInterruptReloadLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGActionPermissionLocalControlPacket Packet)
{
    if (!IsEquipped() || IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGFireRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGReloadStartRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGReloadEndRemoteMessagePacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGFireAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGReloadStartAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

bool ABBBLMGEquipment::QueueInput(FBBBLMGReloadEndAuthorityFactPacket Packet)
{
    if (!IsEquipped() || !IsMirror())
    {
        return false;
    }

    return FBBBLMGParseProcessor::Submit(RuntimeData, MoveTemp(Packet));
}

void ABBBLMGEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}

bool ABBBLMGEquipment::QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet)
{
    return QueueInput(FBBBLMGBlockFireLocalControlPacket{});
}

bool ABBBLMGEquipment::QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet)
{
    return QueueInput(FBBBLMGAllowFireLocalControlPacket{});
}

bool ABBBLMGEquipment::QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet)
{
    return QueueInput(FBBBLMGLoadMagazineLocalControlPacket{});
}

bool ABBBLMGEquipment::QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet)
{
    return QueueInput(FBBBLMGInterruptReloadLocalControlPacket{});
}

bool ABBBLMGEquipment::ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const
{
    Loaded = GetLoadedAmmo();
    Capacity = GetAmmoCapacity();
    bContinuous = true;
    return Capacity > 0;
}
