#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/Core/Shutdown/BBBMeleeShutdown.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ParseSystem/Processors/BBBMeleeParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/AnimationSystem/BBBMeleeAnimationSystem.h"

void FBBBMeleeShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMeleeEquipment &>(BaseEquipment);
    FBBBMeleeActionProcessor::Stop(Equipment.RuntimeData);
    FBBBMeleeAnimationSystem::Stop(Equipment);
    // 停止蒙太奇可同步触发通知 最后清空输入
    FBBBMeleeParseProcessor::Clear(Equipment.RuntimeData);
}
