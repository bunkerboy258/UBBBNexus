
#pragma once

#include "CoreMinimal.h"

struct FBBBCameraCommands;
struct FBBBCharacterAnimationCommands;
struct FBBBCharacterAnimationRequest;

class FBBBCharacterExternalAPI
{
public:

    void Initialize(
        FBBBCharacterAnimationCommands &InAnimationCommands,
        FBBBCameraCommands &InCameraCommands);

    void QueueMontage(const FBBBCharacterAnimationRequest &Request);
    void SubmitCameraRecoil(const FVector2D &Impulse, float RecoverySpeed);
    void SubmitItemIKBlockRequest(bool bBlocked);
private:

    FBBBCharacterAnimationCommands *AnimationCommands = nullptr;
    FBBBCameraCommands *CameraCommands = nullptr;
};
