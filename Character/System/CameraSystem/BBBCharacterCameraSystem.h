
#pragma once
#include "CoreMinimal.h"
class APawn;
class USpringArmComponent;
class FBBBCharacterInitializer;
struct FBBBCharacterCameraConfig;
struct FBBBCharacterWorldRuntimeData;
struct FBBBCameraRuntimeData;
struct FBBBInputRuntimeData;
struct FBBBIntentRuntimeData;

class ABBB_EVAC_API FBBBCharacterCameraSystem final
{
public:

    void Update();
private:
    friend class FBBBCharacterInitializer;

    void Initialize(
        APawn &InPawn,
        USpringArmComponent &InCameraBoom,
        FBBBCameraRuntimeData &InCameraData,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBInputRuntimeData &InInputData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterCameraConfig &InConfig);

    APawn *Pawn = nullptr;

    USpringArmComponent *CameraBoom = nullptr;

    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;

    const FBBBInputRuntimeData *InputData = nullptr;

    const FBBBIntentRuntimeData *IntentData = nullptr;

    FBBBCameraRuntimeData *CameraData = nullptr;

    const FBBBCharacterCameraConfig *Config = nullptr;
};
