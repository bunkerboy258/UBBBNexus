#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/Core/Shutdown/BBBLMGShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ActionSystem/Processors/BBBLMGActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ParseSystem/Processors/BBBLMGParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/AnimationSystem/BBBLMGAnimationSystem.h"

void FBBBLMGShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBLMGEquipment &>(BaseEquipment);
    FBBBLMGActionProcessor::Stop(Equipment.RuntimeData);
    FBBBLMGAnimationSystem::Stop(Equipment);
    // 停止蒙太奇可同步触发通知 最后清空输入
    FBBBLMGParseProcessor::Clear(Equipment.RuntimeData);
}
