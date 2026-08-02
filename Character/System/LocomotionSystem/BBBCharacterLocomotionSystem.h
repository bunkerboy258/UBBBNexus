
#pragma once
#include "CoreMinimal.h"
class APawn;
class UCharacterMovementComponent;
class FBBBCharacterInitializer;
struct FBBBCharacterLocomotionConfig;
struct FBBBIntentRuntimeData;

class ABBB_EVAC_API FBBBCharacterLocomotionSystem final
{
public:

    void Update();
private:
    friend class FBBBCharacterInitializer;

    void Initialize(
        APawn &InPawn,
        UCharacterMovementComponent &InMovement,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterLocomotionConfig &InConfig);

    APawn *Pawn = nullptr;

    UCharacterMovementComponent *Movement = nullptr;

    const FBBBIntentRuntimeData *IntentData = nullptr;

    const FBBBCharacterLocomotionConfig *Config = nullptr;
};
