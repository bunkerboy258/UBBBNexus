
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/States/BBBInputRawData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/States/BBBInputStates.h"
#include "BBBInputRuntimeData.generated.h"
class FBBBCharacterInitializer;
struct FBBBCharacterRuntimeData;

USTRUCT(BlueprintType)
//输入后处理数据
struct FBBBInputRuntimeData
{
    GENERATED_BODY()

    /**
     * 读取当前处理后输入帧
     * @return 处理后输入帧
     */
    const FBBBProcessedInputFrame &GetProcessedInput() const
    {
        return ProcessedInput;
    }

    /**
     * 读取上一轮处理后输入帧
     * @return 上一轮处理后输入帧
     */
    const FBBBProcessedInputFrame &GetLastProcessedInput() const
    {
        return LastProcessedInput;
    }

    /**
     * 读取开火松开宽限剩余时间
     * @return 宽限剩余秒数
     */
    float GetFireRaisedGraceTimer() const
    {
        return FireRaisedGraceTimer;
    }

    /**
     * 读取移动闪断缓冲剩余时间
     * @return 缓冲剩余秒数
     */
    float GetMoveFlickerBufferTimer() const
    {
        return MoveFlickerBufferTimer;
    }

    /**
     * 读取输入处理帧序号
     * @return 帧序号
     */
    int32 GetFrameCounter() const
    {
        return FrameCounter;
    }

    /**
     * 提交本帧处理后输入与容错计时状态
     * @param Processed	处理后输入帧
     * @param InFireRaisedGraceTimer	开火松开宽限计时
     * @param InMoveFlickerBufferTimer	移动闪断缓冲计时
     * @param InFrameCounter	帧序号
     */
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

    friend class FBBBCharacterInitializer;
    friend struct FBBBCharacterRuntimeData;

    //保存增强输入回调直接写入的原始输入
    FBBBInputRawData RawInputData;

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
