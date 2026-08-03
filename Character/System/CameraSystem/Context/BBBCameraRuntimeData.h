#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/Context/BBBCameraCommands.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/Context/BBBCameraStates.h"
#include "BBBCameraRuntimeData.generated.h"
class FBBBCharacterInitializer;

USTRUCT(BlueprintType)
//组装相机领域命令与状态
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
