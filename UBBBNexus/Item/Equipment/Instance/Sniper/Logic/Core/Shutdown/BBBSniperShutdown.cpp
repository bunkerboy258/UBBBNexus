#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/Core/Shutdown/BBBSniperShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ActionSystem/Processors/BBBSniperActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ParseSystem/Processors/BBBSniperParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/AnimationSystem/BBBSniperAnimationSystem.h"

void FBBBSniperShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSniperEquipment &>(BaseEquipment);
    FBBBSniperActionProcessor::Stop(Equipment.RuntimeData);
    FBBBSniperAnimationSystem::Stop(Equipment);
    // 停止蒙太奇可同步触发通知 最后清空输入
    FBBBSniperParseProcessor::Clear(Equipment.RuntimeData);
}
