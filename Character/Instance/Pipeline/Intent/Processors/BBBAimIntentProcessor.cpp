
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Processors/BBBAimIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/States/BBBIntentStates.h"

void FBBBAimIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    // 瞄准意图只使用输入处理器提交的稳定结果
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    // 精准瞄准或开火宽限有效时保持瞄准姿态
    Intent.bWantsAim = Input.bPrecisionAimHeld || Input.bFireRaisedGraceActive;
}
