
#pragma once
#include "CoreMinimal.h"
class APawn;
class USkeletalMeshComponent;
class FBBBCharacterInitializer;
struct FBBBAimAnimationConfig;
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBAimTraceResult;
struct FBBBCharacterAnimationCommands;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterWorldRuntimeData;
struct FBBBIntentRuntimeData;

//计算视线目标骨骼空间瞄准角 与动画目标
class ABBB_EVAC_API FBBBCharacterAimSystem final
{
public:

    void Update();
private:
    friend class FBBBCharacterInitializer;

    void Initialize(
        APawn &InPawn,
        USkeletalMeshComponent &InCharacterMesh,
        FBBBAimRuntimeData &InAimData,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterEquipmentState &InEquipmentState,
        const FBBBCharacterAnimationCommands &InAnimationCommands,
        const FBBBAimConfig &InAimConfig,
        const FBBBAimAnimationConfig &InAnimationConfig);

    bool BuildAimTrace(
        const FVector &ViewLocation,
        const FRotator &ViewRotation,
        AActor *EquippedActor,
        FBBBAimTraceResult &OutResult) const;

    APawn *Pawn = nullptr;
    USkeletalMeshComponent *CharacterMesh = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    FBBBAimRuntimeData *AimData = nullptr;
    const FBBBIntentRuntimeData *IntentData = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;
    const FBBBCharacterAnimationCommands *AnimationCommands = nullptr;
    const FBBBAimConfig *AimConfig = nullptr;
    const FBBBAimAnimationConfig *AnimationConfig = nullptr;
};
