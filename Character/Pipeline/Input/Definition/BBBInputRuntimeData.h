
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Input/BBBInputTypes.h"
#include "BBBInputRuntimeData.generated.h"

USTRUCT(BlueprintType)
//输入后处理数据
struct FBBBInputRuntimeData
{
    GENERATED_BODY()

    const FBBBProcessedInputFrame &GetProcessedInput() const
    {
        return ProcessedInput;
    }

    const FBBBProcessedInputFrame &GetLastProcessedInput() const
    {
        return LastProcessedInput;
    }

    float GetFireRaisedGraceTimer() const
    {
        return FireRaisedGraceTimer;
    }

    float GetMoveFlickerBufferTimer() const
    {
        return MoveFlickerBufferTimer;
    }

    int32 GetFrameCounter() const
    {
        return FrameCounter;
    }

    void CommitProcessedInput(
        const FBBBProcessedInputFrame &Processed,
        float InFireRaisedGraceTimer,
        float InMoveFlickerBufferTimer,
        int32 InFrameCounter)
    {
        ProcessedInput = Processed;
        LastProcessedInput = Processed;
        FireRaisedGraceTimer = InFireRaisedGraceTimer;
        MoveFlickerBufferTimer = InMoveFlickerBufferTimer;
        FrameCounter = InFrameCounter;
    }
    
private:

    UPROPERTY()
    FBBBProcessedInputFrame ProcessedInput;
    //上一轮的
    FBBBProcessedInputFrame LastProcessedInput;

    //开火松开后的短时宽限计时
    float FireRaisedGraceTimer = 0.0f;
    //移动轴短时归零的容错计时
    float MoveFlickerBufferTimer = 0.0f;

    //输入处理次数生成的帧序号
    int32 FrameCounter = 0;
};
