
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Processors/BBBInputProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/States/BBBInputStates.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Input/BBBInputConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/States/BBBInputRawData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"

void FBBBInputProcessor::Update(
    const FBBBInputRawData &InputRawData,
    FBBBInputRuntimeData &InputData,
    const FBBBInputPipelineConfig &Config,
    float DeltaSeconds) const
{
    const FBBBRawInputFrame &Raw = InputRawData.GetAccumulatedRawInput();
    FBBBProcessedInputFrame Processed;

    //分配连续递增的帧序号
    int32 FrameCounter = InputData.GetFrameCounter() + 1;
    Processed.FrameIndex = FrameCounter;

    //继承尚未耗尽的移动闪断缓冲
    float MoveFlickerBufferTimer = InputData.GetMoveFlickerBufferTimer();
    //继承尚未耗尽的开火松开宽限
    float FireRaisedGraceTimer = InputData.GetFireRaisedGraceTimer();


    const bool bRawHasMove = Raw.MoveAxis.SizeSquared() > FMath::Square(Config.MoveDeadZone);

    if (bRawHasMove)
    {
        Processed.MoveInput = Raw.MoveAxis;
        Processed.bHasMoveInput = true;
        //重新填满缓冲以容忍随后短暂归零
        MoveFlickerBufferTimer = Config.MoveFlickerBufferTime;
    }
    else
    {
        MoveFlickerBufferTimer -= DeltaSeconds;
        Processed.bHasMoveInput = MoveFlickerBufferTimer > 0.0f;

        //读取上一帧已处理输入
        const FBBBProcessedInputFrame &LastInput = InputData.GetLastProcessedInput();

        //缓冲期间沿用上一方向否则彻底归零
        Processed.MoveInput = Processed.bHasMoveInput
            ? LastInput.MoveInput
            : FVector2D::ZeroVector;
    }

    //视角增量
    Processed.LookDelta = Raw.LookAxis;
    Processed.bFireHeld = Raw.bFireHeld;
    Processed.bFirePressed = Raw.bFireStarted;
    Processed.bFireReleased = Raw.bFireCompleted;

    //按住开火时持续刷新松开宽限
    if (Raw.bFireHeld)
    {
        FireRaisedGraceTimer = Config.FireRaisedGraceTime;
    }
    else
    {
        FireRaisedGraceTimer -= DeltaSeconds;
    }

    Processed.bFireRaisedGraceActive = FireRaisedGraceTimer > 0.0f;
    
    Processed.bPrecisionAimHeld = Raw.bPrecisionAimHeld;
    Processed.bPrecisionAimPressed = Raw.bPrecisionAimStarted;
    Processed.bPrecisionAimReleased = Raw.bPrecisionAimCompleted;
    
    Processed.bSprintHeld = Raw.bSprintHeld;
    Processed.bReloadPressed = Raw.bReloadPressed;
    Processed.bEquipSlot1Pressed = Raw.bEquipSlot1Pressed;
    Processed.bEquipSlot2Pressed = Raw.bEquipSlot2Pressed;

    InputData.CommitProcessedInput(
        Processed,
        FireRaisedGraceTimer,
        MoveFlickerBufferTimer,
        FrameCounter);
}
