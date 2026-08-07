
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/States/BBBInputStates.h"

struct FBBBInputRawData
{

    /**
     * 写入原始移动轴输入
     * @param Value	移动轴二维值
     */
    void SetMoveAxis(const FVector2D &Value)
    {
        RawInput.MoveAxis = Value;
    }

    /**
     * 写入原始视角轴增量
     * @param Value	视角轴二维值
     */
    void SetLookAxis(const FVector2D &Value)
    {
        RawInput.LookAxis = Value;
    }

    /**
     * 更新开火按住状态
     * @param bHeld	是否按住开火
     */
    void SetFireHeld(bool bHeld)
    {
        RawInput.bFireHeld = bHeld;
    }

    /**
     * 标记本帧开火按下
     */
    void MarkFireStarted()
    {
        RawInput.bFireStarted = true;
    }

    /**
     * 标记本帧开火松开
     */
    void MarkFireCompleted()
    {
        RawInput.bFireCompleted = true;
    }

    /**
     * 更新精确瞄准按住状态
     * @param bHeld	是否按住精确瞄准
     */
    void SetPrecisionAimHeld(bool bHeld)
    {
        RawInput.bPrecisionAimHeld = bHeld;
    }

    /**
     * 标记本帧精确瞄准按下
     */
    void MarkPrecisionAimStarted()
    {
        RawInput.bPrecisionAimStarted = true;
    }

    /**
     * 标记本帧精确瞄准松开
     */
    void MarkPrecisionAimCompleted()
    {
        RawInput.bPrecisionAimCompleted = true;
    }

    /**
     * 更新冲刺按住状态
     * @param bHeld	是否按住冲刺
     */
    void SetSprintHeld(bool bHeld)
    {
        RawInput.bSprintHeld = bHeld;
    }

    /** 标记本帧跳跃按下 */
    void MarkJumpPressed()
    {
        RawInput.bJumpPressed = true;
    }

    /**
     * 标记本帧换弹按下
     */
    void MarkReloadPressed()
    {
        RawInput.bReloadPressed = true;
    }

    /**
     * 标记本帧装备槽位一按下
     */
    void MarkEquipSlot1Pressed()
    {
        RawInput.bEquipSlot1Pressed = true;
    }

    /**
     * 标记本帧装备槽位二按下
     */
    void MarkEquipSlot2Pressed()
    {
        RawInput.bEquipSlot2Pressed = true;
    }

    /**
     * 读取累积的原始输入帧
     * @return 原始输入帧
     */
    const FBBBRawInputFrame &GetAccumulatedRawInput() const
    {
        return RawInput;
    }

    /**
     * 清除不得跨帧保留的增量输入
     */
    void CleanFrameInput()
    {
        //增量 不得跨帧保留
        RawInput.LookAxis = FVector2D::ZeroVector;

        RawInput.bFireStarted = false;

        RawInput.bFireCompleted = false;

        RawInput.bPrecisionAimStarted = false;

        RawInput.bPrecisionAimCompleted = false;

        RawInput.bJumpPressed = false;

        RawInput.bReloadPressed = false;

        RawInput.bEquipSlot1Pressed = false;

        RawInput.bEquipSlot2Pressed = false;
    }
    
private:

    FBBBRawInputFrame RawInput;
    
};
