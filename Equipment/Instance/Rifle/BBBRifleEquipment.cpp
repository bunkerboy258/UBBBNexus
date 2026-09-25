#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Initialization/BBBRifleInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Shutdown/BBBRifleShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/Shared/Action/BBBEquipmentEquipPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/Local/Action/BBBEquipmentPrimaryPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/Local/Action/BBBEquipmentReloadPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkPayload.h"

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

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentEquipPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitShared(RuntimeData, IsEquipped(), Packet);
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentPrimaryPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentReloadPacket Packet)
{
    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::SubmitInput(FBBBRifleDetachMagazinePacket &&Packet)
{
    if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("步枪脱匣输入线程或数据无效")))
    {
        return false;
    }

    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::SubmitInput(FBBBRifleLoadMagazinePacket &&Packet)
{
    if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("步枪装匣输入线程或数据无效")))
    {
        return false;
    }

    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

bool ABBBRifleEquipment::SubmitInput(FBBBRifleInterruptReloadPacket &&Packet)
{
    if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("步枪换弹结束输入线程或数据无效")))
    {
        return false;
    }

    return FBBBRifleParseProcessor::SubmitLocal(RuntimeData, IsEquipped(), IsMirror(), Packet);
}

void ABBBRifleEquipment::OnUnequipped()
{
    FBBBRifleShutdown::Shutdown(*this);
}

bool ABBBRifleEquipment::QueueInput(FBBBEquipmentNetworkPayload Payload)
{
    return FBBBRifleParseProcessor::SubmitMirror(RuntimeData, IsEquipped(), IsMirror(), Payload);
}

void ABBBRifleEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}
