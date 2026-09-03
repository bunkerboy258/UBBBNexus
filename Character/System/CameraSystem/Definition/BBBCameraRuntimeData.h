#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/Definition/States/BBBCameraStates.h"
#include "BBBCameraRuntimeData.generated.h"
class FBBBCharacterInitializer;

USTRUCT(BlueprintType)
//组装相机领域命令与状态
struct FBBBCameraRuntimeData
{
    GENERATED_BODY()

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
    FBBBCameraState State;
};
