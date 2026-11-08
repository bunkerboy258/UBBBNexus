#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/Core/Shutdown/BBBRevolverShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/Processors/BBBRevolverActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ParseSystem/Processors/BBBRevolverParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/AnimationSystem/BBBRevolverAnimationSystem.h"

void FBBBRevolverShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBRevolverEquipment &>(BaseEquipment);
    FBBBRevolverActionProcessor::Stop(Equipment.RuntimeData);
    FBBBRevolverAnimationSystem::Stop(Equipment);
    // 停止蒙太奇可同步触发通知 最后清空输入
    FBBBRevolverParseProcessor::Clear(Equipment.RuntimeData);
}
