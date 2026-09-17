
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Processors/BBBEquipmentSelectionIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/States/BBBIntentStates.h"

void FBBBEquipmentSelectionIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    // 装备选择只读取规范化后的本帧按键结果
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    // 一号槽位优先处理保证同帧输入有确定结果
    if (Input.bEquipSlot1Pressed)
    {
        Intent.RequestedEquipSlot = 0;

        // 已经确定槽位后结束处理避免二号槽位覆盖结果
        return;
    }

    // 只有一号槽位未触发时才检查二号槽位
    if (Input.bEquipSlot2Pressed)
    {
        Intent.RequestedEquipSlot = 1;
    }
}
