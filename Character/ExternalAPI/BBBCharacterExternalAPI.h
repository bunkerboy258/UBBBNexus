
#pragma once

#include "CoreMinimal.h"

struct FBBBCameraCommands;
struct FBBBCharacterAnimationCommands;
struct FBBBCharacterAnimationRequest;
struct FBBBCharacterItemEvents;

class FBBBCharacterExternalAPI
{
public:

    void Initialize(
        FBBBCharacterAnimationCommands &InAnimationCommands,
        FBBBCameraCommands &InCameraCommands,
        FBBBCharacterItemEvents &InItemEvents);

    void QueueMontage(const FBBBCharacterAnimationRequest &Request);
    void SubmitCameraRecoil(const FVector2D &Impulse, float RecoverySpeed);
    void PublishItemFireEvent();
    void PublishReloadEvent();
    void SubmitItemIKBlockRequest(bool bBlocked);
private:

    FBBBCharacterAnimationCommands *AnimationCommands = nullptr;
    FBBBCameraCommands *CameraCommands = nullptr;
    FBBBCharacterItemEvents *ItemEvents = nullptr;
};
