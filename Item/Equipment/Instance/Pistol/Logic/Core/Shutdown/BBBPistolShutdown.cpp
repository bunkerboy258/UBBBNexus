#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/Core/Shutdown/BBBPistolShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/Processors/BBBPistolActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ParseSystem/Processors/BBBPistolParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/AnimationSystem/BBBPistolAnimationSystem.h"

void FBBBPistolShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBPistolEquipment &>(BaseEquipment);
    FBBBPistolActionProcessor::Stop(Equipment.RuntimeData);
    FBBBPistolAnimationSystem::Stop(Equipment);
    // 停止蒙太奇可同步触发通知 最后清空输入
    FBBBPistolParseProcessor::Clear(Equipment.RuntimeData);
}
