
#pragma once
#include "CoreMinimal.h"
class APawn;
class UCharacterMovementComponent;
class FBBBCharacterInitializer;
struct FBBBAimRuntimeData;
struct FBBBCharacterFacingConfig;
struct FBBBCharacterWorldRuntimeData;
struct FBBBFacingRuntimeData;
struct FBBBIntentRuntimeData;

class ABBB_EVAC_API FBBBCharacterFacingSystem final
{
public:

    void Update();
private:
    friend class FBBBCharacterInitializer;

    void Initialize(
        APawn &InPawn,
        UCharacterMovementComponent &InMovement,
        FBBBFacingRuntimeData &InFacingData,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBAimRuntimeData &InAimData,
        const FBBBCharacterFacingConfig &InConfig);

    APawn *Pawn = nullptr;

    UCharacterMovementComponent *Movement = nullptr;

    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;

    FBBBFacingRuntimeData *FacingData = nullptr;

    const FBBBIntentRuntimeData *IntentData = nullptr;

    const FBBBAimRuntimeData *AimData = nullptr;

    const FBBBCharacterFacingConfig *Config = nullptr;
};
