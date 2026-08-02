
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

    FBBBCameraCommands ConsumeRecoilRequest()
    {
        const FBBBCameraCommands PendingCommands = Commands;
        Commands = FBBBCameraCommands();
        return PendingCommands;
    }

    const FBBBCameraState &GetState() const
    {
        return State;
    }

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
