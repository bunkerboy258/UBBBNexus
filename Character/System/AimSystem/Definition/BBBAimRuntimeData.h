
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeTypes.h"
#include "BBBAimRuntimeData.generated.h"
class FBBBAimRestoreProcessor;
class FBBBCharacterAimSystem;

//远端传来的瞄准状态
struct FBBBRemoteAimState
{
    bool bIsAiming = false;

    FVector AimTargetWorld = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
//瞄准状态
struct FBBBAimRuntimeData
{
    GENERATED_BODY()

    const FBBBAimRuntimeState &GetState() const
    {
        return State;
    }

    const FVector &GetAimOriginWorld() const
    {
        return AimOriginWorld;
    }
private:
    friend class FBBBAimRestoreProcessor;
    friend class FBBBCharacterAimSystem;

    void CommitLocalState(
        const FBBBAimRuntimeState &InState,
        const FVector &InAimOriginWorld)
    {
        State = InState;
        AimOriginWorld = InAimOriginWorld;
    }

    void RestoreRemoteState(const FBBBRemoteAimState &RemoteState)
    {
        State.bIsAiming = RemoteState.bIsAiming;
        State.AimTargetWorld = RemoteState.AimTargetWorld;
        State.AimIKTargetWorld = RemoteState.AimTargetWorld;
    }

    UPROPERTY(Transient)
    FBBBAimRuntimeState State;

    UPROPERTY(Transient)
    FVector AimOriginWorld = FVector::ZeroVector;
};
