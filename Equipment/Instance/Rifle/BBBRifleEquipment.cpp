#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Initialization/BBBRifleInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Shutdown/BBBRifleShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/BBBRifleNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Action/FBBBRifleActionStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentStateAuthorityFactPacket.h"

ABBBRifleEquipment::ABBBRifleEquipment()
{
    FBBBRifleInitializer::ConfigureTick(*this);
}

bool ABBBRifleEquipment::InitializeRuntimeData()
{
    return FBBBRifleInitializer::Initialize(*this);
}

void ABBBRifleEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBRifleUpdatePipeline::Update(*this);
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitEquip(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitEquip(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentReloadLocalControlPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleDetachMagazineLocalControlPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleLoadMagazineLocalControlPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::QueueInput(FBBBRifleInterruptReloadLocalControlPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

void ABBBRifleEquipment::OnUnequipped()
{
    FBBBRifleShutdown::Shutdown(*this);
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentStateAuthorityFactPacket Payload)
{
    FBBBRifleActionStateAuthorityFactPacket Packet;
    if (!FBBBRifleNetworkSystem::DecodeAuthorityFact(RuntimeData, Payload.Data, Packet))
    {
        return false;
    }

    return FBBBRifleParseProcessor::SubmitAuthorityFact(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

void ABBBRifleEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}
