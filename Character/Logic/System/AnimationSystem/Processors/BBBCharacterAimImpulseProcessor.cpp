#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterAimImpulseProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"

void FBBBCharacterAimImpulseProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Animation.AimImpulseState;
    auto &Facts = Context.RuntimeData.Animation.AnimationFactState;
    const auto &Config = Context.AnimationConfig;
    const float DeltaSeconds = Context.WorldState.FrameDeltaSeconds;
    if (!ensureMsgf(
        FMath::IsFinite(State.RecoverySpeed) && State.RecoverySpeed > 0.0f
            && !Config.AimImpulseLimitDegrees.ContainsNaN()
            && Config.AimImpulseLimitDegrees.X > 0.0f && Config.AimImpulseLimitDegrees.Y > 0.0f,
        TEXT("角色瞄准冲击恢复速度或角度限制无效")))
    {
        State.PendingDegrees = FVector2D::ZeroVector;
        State.OffsetDegrees = FVector2D::ZeroVector;
        Facts.AimOffsetDegrees = FVector2D::ZeroVector;
        return;
    }

    if (!Context.RuntimeData.Equipment.ReadEquipmentSelectionState().ActiveMainHandInstance)
    {
        State.PendingDegrees = FVector2D::ZeroVector;
        State.OffsetDegrees = FVector2D::ZeroVector;
        Facts.AimOffsetDegrees = FVector2D::ZeroVector;
        return;
    }

    State.OffsetDegrees *= FMath::Exp(-State.RecoverySpeed * FMath::Max(DeltaSeconds, 0.0f));
    State.OffsetDegrees += State.PendingDegrees;
    State.PendingDegrees = FVector2D::ZeroVector;
    State.OffsetDegrees.X = FMath::Clamp(State.OffsetDegrees.X, -Config.AimImpulseLimitDegrees.X, Config.AimImpulseLimitDegrees.X);
    State.OffsetDegrees.Y = FMath::Clamp(State.OffsetDegrees.Y, -Config.AimImpulseLimitDegrees.Y, Config.AimImpulseLimitDegrees.Y);
    Facts.AimOffsetDegrees = State.OffsetDegrees;
    if (!ensureMsgf(!Facts.AimOffsetDegrees.ContainsNaN(), TEXT("角色后坐力大小无效")))
    {
        State.PendingDegrees = FVector2D::ZeroVector;
        State.OffsetDegrees = FVector2D::ZeroVector;
        Facts.AimOffsetDegrees = FVector2D::ZeroVector;
        return;
    }
}
