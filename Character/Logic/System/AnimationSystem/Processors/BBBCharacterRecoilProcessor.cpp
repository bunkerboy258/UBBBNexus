#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterRecoilProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"

void FBBBCharacterRecoilProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Animation.RecoilState;
    auto &Facts = Context.RuntimeData.Animation.AnimationFactState;
    auto *Source = Context.AnimationInstance.TryGetWeaponAnimInstance();
    Facts.RecoilAngles = FVector2D::ZeroVector;
    if (!Source || Source->GetRecoilPattern().IsEmpty())
    {
        State.Source.Reset();
        State.FireSequence = 0;
        State.ShotCount = 0;
        State.OffsetDegrees = FVector2D::ZeroVector;
        State.bStartNewSegment = true;
        return;
    }

    const int32 Sequence = Source->GetFireSequence();
    if (State.Source.Get() != Source)
    {
        State.Source = Source;
        State.FireSequence = Sequence;
        State.ShotCount = 0;
        State.OffsetDegrees = FVector2D::ZeroVector;
        State.bStartNewSegment = true;
    }

    float Scale = Facts.bIsAiming ? Source->GetAimRecoilScale() : Source->GetHipRecoilScale();
    if (Facts.bIsCrouching)
    {
        Scale *= Source->GetCrouchedRecoilScale();
    }
    if (Facts.MovementMode == MOVE_Falling)
    {
        Scale *= Source->GetAirborneRecoilScale();
    }
    const float Speed = Source->GetRecoilReturnSpeed();
    if (!ensureMsgf(FMath::IsFinite(Scale) && Scale >= 0.0f && FMath::IsFinite(Speed) && Speed > 0.0f,
        TEXT("后坐力轨迹倍率或回落速度无效")))
    {
        State.OffsetDegrees = FVector2D::ZeroVector;
        State.ShotCount = 0;
        State.FireSequence = Sequence;
        return;
    }

    const bool bHolding = Source->HoldsRecoilBetweenShots() && Facts.bFire
        && Source->CanSustainRecoil() && !Source->IsReloading();
    const int32 Shots = FMath::Max(Sequence - State.FireSequence, 0);
    if (Source->IsReloading())
    {
        State.ShotCount = 0;
    }
    if (Shots > 0 && !Source->IsReloading())
    {
        if (Source->HoldsRecoilBetweenShots() && State.bStartNewSegment)
        {
            State.ShotCount = 0;
        }
        const auto Pattern = Source->GetRecoilPattern();
        State.bStartNewSegment = false;
        State.ShotCount = FMath::Min(State.ShotCount + Shots, Pattern.Num());
        State.OffsetDegrees = Pattern[State.ShotCount - 1];
        if (!ensureMsgf(!State.OffsetDegrees.ContainsNaN(), TEXT("后坐力轨迹包含无效角度")))
        {
            State.OffsetDegrees = FVector2D::ZeroVector;
        }
    }
    if (Shots == 0 && !bHolding)
    {
        const double Length = State.OffsetDegrees.Size();
        const double Step = Speed * FMath::Max(Context.WorldState.FrameDeltaSeconds, 0.0f)
            / FMath::Max(Scale, UE_SMALL_NUMBER);
        State.OffsetDegrees *= FMath::Max(0.0, Length - Step) / FMath::Max(Length, UE_SMALL_NUMBER);
        if (State.OffsetDegrees.IsNearlyZero())
        {
            State.OffsetDegrees = FVector2D::ZeroVector;
            State.ShotCount = 0;
        }
    }
    State.FireSequence = Sequence;
    if (Source->HoldsRecoilBetweenShots() && !bHolding)
    {
        State.bStartNewSegment = true;
    }
    const FVector2D Limit = Context.AnimationConfig.RecoilLimitDegrees;
    const FVector2D Angles = State.OffsetDegrees * Scale;
    Facts.RecoilAngles.X = FMath::Clamp(Angles.X, -Limit.X, Limit.X);
    Facts.RecoilAngles.Y = FMath::Clamp(Angles.Y, -Limit.Y, Limit.Y);
}
