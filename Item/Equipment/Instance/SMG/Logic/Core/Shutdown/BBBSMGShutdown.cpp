#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/Core/Shutdown/BBBSMGShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/Processors/BBBSMGActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ParseSystem/Processors/BBBSMGParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/AnimationSystem/BBBSMGAnimationSystem.h"

void FBBBSMGShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSMGEquipment &>(BaseEquipment);
    FBBBSMGActionProcessor::Stop(Equipment.RuntimeData);
    FBBBSMGAnimationSystem::Stop(Equipment);
    // 停止蒙太奇可同步触发通知 最后清空输入
    FBBBSMGParseProcessor::Clear(Equipment.RuntimeData);
}
