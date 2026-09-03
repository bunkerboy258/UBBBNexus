
#pragma once
#include "CoreMinimal.h"
#include "BBBInputStates.generated.h"

USTRUCT(BlueprintType)
//保存输入回调直接写入的设备状态与单帧边沿
struct FBBBRawInputFrame
{
    GENERATED_BODY()

    //保存移动动作当前二维轴值
    UPROPERTY(BlueprintReadOnly)
    FVector2D MoveAxis = FVector2D::ZeroVector;

    //保存视角动作当前帧二维增量
    UPROPERTY(BlueprintReadOnly)
    FVector2D LookAxis = FVector2D::ZeroVector;

    //标记开火按键当前保持状态
    UPROPERTY(BlueprintReadOnly)
    bool bFireHeld = false;

    //标记开火按键本帧产生上升沿
    UPROPERTY(BlueprintReadOnly)
    bool bFireStarted = false;

    //标记开火按键本帧产生下降沿
    UPROPERTY(BlueprintReadOnly)
    bool bFireCompleted = false;

    //标记精确瞄准按键当前保持状态
    UPROPERTY(BlueprintReadOnly)
    bool bPrecisionAimHeld = false;

    //标记精确瞄准按键本帧产生上升沿
    UPROPERTY(BlueprintReadOnly)
    bool bPrecisionAimStarted = false;

    //标记精确瞄准按键本帧产生下降沿
    UPROPERTY(BlueprintReadOnly)
    bool bPrecisionAimCompleted = false;

    /** 标记行走按键当前保持状态 */
    UPROPERTY(BlueprintReadOnly)
    bool bWalkHeld = false;

    //标记冲刺按键当前保持状态
    UPROPERTY(BlueprintReadOnly)
    bool bSprintHeld = false;

    /** 标记蹲伏按键当前保持状态 */
    UPROPERTY(BlueprintReadOnly)
    bool bCrouchHeld = false;

    /** 标记跳跃按键本帧被按下 */
    UPROPERTY(BlueprintReadOnly)
    bool bJumpPressed = false;

    //标记换弹按键本帧被按下
    UPROPERTY(BlueprintReadOnly)
    bool bReloadPressed = false;

    //标记一号装备槽按键本帧被按下
    UPROPERTY(BlueprintReadOnly)
    bool bEquipSlot1Pressed = false;

    //标记二号装备槽按键本帧被按下
    UPROPERTY(BlueprintReadOnly)
    bool bEquipSlot2Pressed = false;
};

USTRUCT(BlueprintType)
//保存输入管线发布给意图阶段的稳定帧快照
struct FBBBProcessedInputFrame
{
    GENERATED_BODY()

    //记录该快照的连续输入帧序号
    UPROPERTY(BlueprintReadOnly)
    int32 FrameIndex = 0;

    //保存经过死区与闪断处理的移动输入
    UPROPERTY(BlueprintReadOnly)
    FVector2D MoveInput = FVector2D::ZeroVector;

    //保存当前帧需要消费的视角增量
    UPROPERTY(BlueprintReadOnly)
    FVector2D LookDelta = FVector2D::ZeroVector;

    //标记当前帧是否存在有效或缓冲移动
    UPROPERTY(BlueprintReadOnly)
    bool bHasMoveInput = false;

    //保存开火按键当前保持状态
    UPROPERTY(BlueprintReadOnly)
    bool bFireHeld = false;

    //保存开火按键本帧上升沿
    UPROPERTY(BlueprintReadOnly)
    bool bFirePressed = false;

    //保存开火按键本帧下降沿
    UPROPERTY(BlueprintReadOnly)
    bool bFireReleased = false;

    //标记开火松开后 瞄准宽限是否有效
    UPROPERTY(BlueprintReadOnly)
    bool bFireRaisedGraceActive = false;

    //保存精确瞄准按键当前保持状态
    UPROPERTY(BlueprintReadOnly)
    bool bPrecisionAimHeld = false;

    //保存精确瞄准按键本帧上升沿
    UPROPERTY(BlueprintReadOnly)
    bool bPrecisionAimPressed = false;

    //保存精确瞄准按键本帧下降沿
    UPROPERTY(BlueprintReadOnly)
    bool bPrecisionAimReleased = false;

    /** 保存行走按键当前保持状态 */
    UPROPERTY(BlueprintReadOnly)
    bool bWalkHeld = false;

    //保存冲刺按键当前保持状态
    UPROPERTY(BlueprintReadOnly)
    bool bSprintHeld = false;

    /** 保存蹲伏按键当前保持状态 */
    UPROPERTY(BlueprintReadOnly)
    bool bCrouchHeld = false;

    /** 保存本帧一次性跳跃输入 */
    UPROPERTY(BlueprintReadOnly)
    bool bJumpPressed = false;

    //保存本帧一次性换弹请求
    UPROPERTY(BlueprintReadOnly)
    bool bReloadPressed = false;

    //保存本帧一号槽切换请求
    UPROPERTY(BlueprintReadOnly)
    bool bEquipSlot1Pressed = false;

    //保存本帧二号槽切换请求
    UPROPERTY(BlueprintReadOnly)
    bool bEquipSlot2Pressed = false;
};
