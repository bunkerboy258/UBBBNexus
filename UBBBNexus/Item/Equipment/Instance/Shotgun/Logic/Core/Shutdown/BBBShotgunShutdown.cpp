#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/Core/Shutdown/BBBShotgunShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/Processors/BBBShotgunActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ParseSystem/Processors/BBBShotgunParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/BBBShotgunAnimationSystem.h"

void FBBBShotgunShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBShotgunEquipment &>(BaseEquipment);
    FBBBShotgunActionProcessor::Stop(Equipment.RuntimeData);
    FBBBShotgunAnimationSystem::Stop(Equipment);
    // 停止蒙太奇可同步触发通知 最后清空输入
    FBBBShotgunParseProcessor::Clear(Equipment.RuntimeData);
}
