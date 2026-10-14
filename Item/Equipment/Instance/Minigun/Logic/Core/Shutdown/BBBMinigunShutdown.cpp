#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/Core/Shutdown/BBBMinigunShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ActionSystem/Processors/BBBMinigunActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ParseSystem/Processors/BBBMinigunParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/AnimationSystem/BBBMinigunAnimationSystem.h"

void FBBBMinigunShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMinigunEquipment &>(BaseEquipment);
    FBBBMinigunActionProcessor::Stop(Equipment.RuntimeData);
    FBBBMinigunAnimationSystem::Stop(Equipment);
    // 停止蒙太奇可同步触发通知 最后清空输入
    FBBBMinigunParseProcessor::Clear(Equipment.RuntimeData);
}
