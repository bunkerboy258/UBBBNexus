
#pragma once
#include "CoreMinimal.h"
class APawn;
class FBBBArbitrationPipeline;
class FBBBCharacterAimSystem;
class FBBBCharacterAnimationSystem;
class FBBBCharacterCameraSystem;
class FBBBCharacterFacingSystem;
class FBBBCharacterItemSystem;
class FBBBCharacterLocomotionSystem;
class FBBBCharacterNetworkSystem;
class FBBBCharacterInitializer;
class FBBBExecutionPipeline;
class FBBBInputPipeline;
class FBBBIntentPipeline;
class FBBBRequestPipeline;
struct FBBBCharacterRuntimeData;

class ABBB_EVAC_API FBBBCharacterUpdatePipeline final
{
public:
    
    void Update() const;
    
private:
    
    friend class FBBBCharacterInitializer;

    void Initialize(
        APawn &InPawn,
        FBBBCharacterRuntimeData &InRuntimeData,
        FBBBCharacterCameraSystem &InCameraSystem,
        FBBBCharacterAimSystem &InAimSystem,
        FBBBCharacterLocomotionSystem &InLocomotionSystem,
        FBBBCharacterFacingSystem &InFacingSystem,
        FBBBCharacterItemSystem &InItemSystem,
        FBBBCharacterNetworkSystem &InNetworkSystem,
        FBBBCharacterAnimationSystem &InAnimationSystem,
        FBBBInputPipeline &InInputPipeline,
        FBBBIntentPipeline &InIntentPipeline,
        FBBBRequestPipeline &InRequestPipeline,
        FBBBArbitrationPipeline &InArbitrationPipeline,
        FBBBExecutionPipeline &InExecutionPipeline);

    void UpdateLocalAuthority() const;

    void UpdateLocalAutonomous() const;

    void UpdateRemoteAuthority() const;

    void UpdateRemoteSimulated() const;

    APawn *Pawn = nullptr;

    FBBBCharacterRuntimeData *RuntimeData = nullptr;

    FBBBCharacterCameraSystem *CameraSystem = nullptr;

    FBBBCharacterAimSystem *AimSystem = nullptr;

    FBBBCharacterLocomotionSystem *LocomotionSystem = nullptr;

    FBBBCharacterFacingSystem *FacingSystem = nullptr;

    FBBBCharacterItemSystem *ItemSystem = nullptr;

    FBBBCharacterNetworkSystem *NetworkSystem = nullptr;

    FBBBCharacterAnimationSystem *AnimationSystem = nullptr;

    FBBBInputPipeline *InputPipeline = nullptr;

    FBBBIntentPipeline *IntentPipeline = nullptr;

    FBBBRequestPipeline *RequestPipeline = nullptr;

    FBBBArbitrationPipeline *ArbitrationPipeline = nullptr;

    FBBBExecutionPipeline *ExecutionPipeline = nullptr;
};
