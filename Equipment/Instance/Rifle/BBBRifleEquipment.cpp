#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Initialization/BBBRifleInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Shutdown/BBBRifleShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/Processors/BBBRifleNetworkProcessor.h"

ABBBRifleEquipment::ABBBRifleEquipment()
{
    FBBBRifleUpdatePipeline::ConfigureTick(*this);
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

void ABBBRifleEquipment::SubmitEquipInput()
{
    if (IsEquipped())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleEquipPacket{});
    }
}

void ABBBRifleEquipment::SubmitPrimaryInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleFirePacket{});
    }
}

void ABBBRifleEquipment::SubmitReloadInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleReloadPacket{});
    }
}

void ABBBRifleEquipment::SubmitDetachMagazineInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleDetachMagazinePacket{});
    }
}

void ABBBRifleEquipment::SubmitLoadMagazineInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleLoadMagazinePacket{});
    }
}

void ABBBRifleEquipment::SubmitInterruptReloadInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleInterruptReloadPacket{});
    }
}

void ABBBRifleEquipment::OnUnequipped()
{
    FBBBRifleShutdown::Shutdown(*this);
}

bool ABBBRifleEquipment::SubmitNetworkPayload(const TArray<uint8> &Data)
{
    return IsEquipped() && IsMirror() && FBBBRifleNetworkProcessor::Submit(RuntimeData, Data);
}

void ABBBRifleEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}
