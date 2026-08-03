
#pragma once
#include "CoreMinimal.h"
#include "BBBCameraRuntimeData.generated.h"
class FBBBCharacterExternalAPI;
class FBBBCharacterCameraSystem;
class FBBBCharacterInitializer;
struct FBBBCameraRuntimeData;

USTRUCT(BlueprintType)
struct FBBBCameraCommands
{
    GENERATED_BODY()
private:
    friend class FBBBCharacterExternalAPI;
    friend class FBBBCharacterCameraSystem;
    friend struct FBBBCameraRuntimeData;

    /**
     * 提交一次相机后座力请求 冲量累加等待消费
     * @param Impulse	后座力冲量 X为Pitch Y为Yaw
     * @param RecoverySpeed	后座力恢复速度
     */
    void SubmitRecoilRequest(
        const FVector2D &Impulse,
        float RecoverySpeed)
    {
        PendingRecoilImpulse += Impulse;
        PendingRecoilRecoverySpeed = RecoverySpeed;
    }

    UPROPERTY()
    FVector2D PendingRecoilImpulse = FVector2D::ZeroVector;

    UPROPERTY()
    float PendingRecoilRecoverySpeed = 0.0f;
};

USTRUCT(BlueprintType)
struct FBBBCameraState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FVector2D AppliedRecoilOffset = FVector2D::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    float RecoilRecoverySpeed = 8.0f;
};

USTRUCT(BlueprintType)
struct FBBBCameraRuntimeData
{
    GENERATED_BODY()

    /**
     * 取出待处理的后座力请求并清空命令缓冲
     * @return 待处理的后座力请求
     */
    FBBBCameraCommands ConsumeRecoilRequest()
    {
        const FBBBCameraCommands PendingCommands = Commands;
        Commands = FBBBCameraCommands();
        return PendingCommands;
    }

    /**
     * 读取当前相机状态
     * @return 相机状态
     */
    const FBBBCameraState &GetState() const
    {
        return State;
    }

    /**
     * 提交相机状态
     * @param InState	相机状态
     */
    void CommitState(const FBBBCameraState &InState)
    {
        State = InState;
    }
private:
    friend class FBBBCharacterInitializer;

    UPROPERTY()
    FBBBCameraCommands Commands;

    UPROPERTY()
    FBBBCameraState State;
};
