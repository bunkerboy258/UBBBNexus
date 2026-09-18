#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBCharacterInputPackets.generated.h"

class UAnimMontage;

/** 换弹动画反馈阶段 */
UENUM(BlueprintType)
enum class EBBBCharacterReloadPhase : uint8
{
    None,
    DetachMagazine,
    LoadMagazine,
    Interrupted
};

/** 换弹播放结束的原因 */
UENUM(BlueprintType)
enum class EBBBCharacterReloadEndReason : uint8
{
    Loaded,
    Interrupted,
    PlaybackFailed
};

/** 外部控制源提交的持续状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterContinuousInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector MoveWorld = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator FacingWorld = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector AimTargetWorld = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAim = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bWalk = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSprint = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCrouch = false;
};

/** 外部动作和装备事实共用的离散输入 */
USTRUCT(BlueprintType)
struct FBBBCharacterDiscreteInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 EquipSlot = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFire = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bReload = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bJump = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBBBCharacterReloadPhase ReloadPhase = EBBBCharacterReloadPhase::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Sequence = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBEquipmentActionEvent EquipmentEvent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimMontage> Montage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MontagePlayRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bReloadMontage = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D CameraImpulse = FVector2D::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CameraRecoverySpeed = 0.0f;
};

/** 网络带来的已确认离散事实 */
USTRUCT()
struct FBBBCharacterRestoreDiscreteInput
{
    GENERATED_BODY()

    UPROPERTY()
    FName EquipmentHandle = NAME_None;

    UPROPERTY()
    FBBBEquipmentActionEvent EquipmentEvent;

    TOptional<FBBBAimRuntimeState> Aim;

    TOptional<EBBBCharacterGait> Gait;
};
