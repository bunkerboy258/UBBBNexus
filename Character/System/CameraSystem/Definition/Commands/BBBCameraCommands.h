#pragma once
#include "CoreMinimal.h"
#include "BBBCameraCommands.generated.h"
class FBBBCharacterCameraSystem;
class FBBBCharacterExternalAPI;
struct FBBBCameraRuntimeData;

USTRUCT(BlueprintType)
//保存相机系统等待消费的命令
struct FBBBCameraCommands
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterExternalAPI;
    friend class FBBBCharacterCameraSystem;
    friend struct FBBBCameraRuntimeData;

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
