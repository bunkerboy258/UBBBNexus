
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Processors/BBBInputIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/States/BBBIntentStates.h"

void FBBBInputIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    // 意图层只读取输入处理器提交的稳定结果
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    Intent.MoveInput = Input.MoveInput;
    Intent.bHasMoveInput = Input.bHasMoveInput;
    // 冲刺意图存在时不同时产生行走意图
    Intent.bWantsWalk = Input.bWalkHeld && !Input.bSprintHeld;
    Intent.bWantsSprint = Input.bSprintHeld;
    Intent.bWantsCrouch = Input.bCrouchHeld;
    // 跳跃冲刺和滑铲只在按键帧产生脉冲意图
    Intent.bWantsJump = Input.bJumpPressed;
    Intent.bWantsDash = Input.bDashPressed;
    Intent.bWantsSlide = Input.bSlidePressed;
}
