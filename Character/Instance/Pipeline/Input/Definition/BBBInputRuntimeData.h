#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBControlBehaviors.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBEquipBehavior.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBFireBehavior.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBReloadBehavior.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterAnimationInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterRestoreInput.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"
#include "BBBInputRuntimeData.generated.h"
class FBBBCharacterInput;
class FBBBInputPipeline;
class FBBBArbitrationPipeline;

/** 输入入口持有的待消费数据与当前批次 */
USTRUCT()
struct FBBBInputBatch
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FBBBEquipInput> Equip;
    UPROPERTY()
    TArray<FBBBFireInput> Fire;
    UPROPERTY()
    TArray<FBBBReloadInput> Reload;
    UPROPERTY()
    TArray<FBBBJumpInput> Jump;
    UPROPERTY()
    TArray<FBBBEquipmentActionEvent> Results;
    UPROPERTY()
    TArray<FBBBCharacterReloadAnimationInput> Notifications;
    UPROPERTY()
    TArray<FBBBCharacterMontagePacket> Montages;
    UPROPERTY()
    TArray<FBBBCharacterRestoreInput> Restores;
    UPROPERTY()
    TArray<FBBBPlayerCameraInput> Camera;
};

/** 持续控制收件箱只保存各语义输入的最后一次采样 */
USTRUCT()
struct FBBBContinuousInputState
{
    GENERATED_BODY()

    UPROPERTY()
    FBBBMoveInput Move;

    UPROPERTY()
    FBBBViewInput View;

    UPROPERTY()
    FBBBAimInput Aim;

    UPROPERTY()
    FBBBWalkInput Walk;

    UPROPERTY()
    FBBBSprintInput Sprint;

    UPROPERTY()
    FBBBCrouchInput Crouch;
};

/** 输入收件箱不在帧末清理以保留动画后装备提交的结果 */
USTRUCT()
struct FBBBInputRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterInput;
    friend class FBBBInputPipeline;
    friend class FBBBArbitrationPipeline;

    UPROPERTY()
    FBBBContinuousInputState Continuous;
    UPROPERTY()
    FBBBContinuousInputState FrameContinuous;
    UPROPERTY()
    FBBBInputBatch Pending;
    UPROPERTY()
    FBBBInputBatch Frame;
};
