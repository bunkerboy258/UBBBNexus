
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Processors/BBBEquipmentActionIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/States/BBBIntentStates.h"

void FBBBEquipmentActionIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    // 装备动作意图读取同一份稳定输入保证本帧状态一致
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    // 按住开火时持续产生开火意图
    Intent.bWantsFire = Input.bFireHeld;
    // 按下换弹时产生一次换弹意图
    Intent.bWantsReload = Input.bReloadPressed;
}
