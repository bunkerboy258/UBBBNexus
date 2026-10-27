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
    if (!Source || Source->GetRecoilPitchCurve().IsEmpty() || Source->GetRecoilYawCurve().IsEmpty())
    {
        State.Source.Reset();
        State.FireSequence = 0;
        State.ShotCount = 0;
        State.OffsetDegrees = FVector2D::ZeroVector;
        State.CurveAnchorDegrees = FVector2D::ZeroVector;
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
        State.CurveAnchorDegrees = FVector2D::ZeroVector;
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
    const float FireInterval = Source->GetRecoilFireInterval();
    const float SinceLastFire = Source->GetTimeSinceLastFireSeconds();
    if (!ensureMsgf(FMath::IsFinite(Scale) && Scale >= 0.0f && FMath::IsFinite(Speed) && Speed > 0.0f
        && FMath::IsFinite(FireInterval) && FireInterval > 0.0f
        && FMath::IsFinite(SinceLastFire) && SinceLastFire >= 0.0f,
        TEXT("后坐力轨迹倍率 回落速度或射击时间无效")))
    {
        State.OffsetDegrees = FVector2D::ZeroVector;
        State.CurveAnchorDegrees = FVector2D::ZeroVector;
        State.ShotCount = 0;
        State.FireSequence = Sequence;
        State.bStartNewSegment = true;
        return;
    }

    const bool bAutomatic = Source->HoldsRecoilBetweenShots();
    const bool bHolding = bAutomatic && Facts.bFire
        && Source->CanSustainRecoil() && !Source->IsReloading();
    const int32 Shots = FMath::Max(Sequence - State.FireSequence, 0);
    if (Source->IsReloading())
    {
        State.ShotCount = 0;
    }
    if (Shots > 0 && !Source->IsReloading())
    {
        if (bAutomatic && State.bStartNewSegment)
        {
            State.ShotCount = 0;
        }
        if (!bAutomatic || State.bStartNewSegment)
        {
            const float Start = static_cast<float>(State.ShotCount);
            State.CurveAnchorDegrees = State.OffsetDegrees - FVector2D(
                Source->GetRecoilPitchCurve().Eval(Start), Source->GetRecoilYawCurve().Eval(Start));
        }
        State.ShotCount += Shots;
        State.bStartNewSegment = false;
    }

    // 只在实际射击后沿当前枪间区间求值 松键或不能继续射击后立即回落
    const bool bFollowCurve = State.ShotCount > 0 && Facts.bFire && !Source->IsReloading()
        && (Source->CanSustainRecoil() || Shots > 0)
        && (bAutomatic || SinceLastFire < FireInterval);
    if (bFollowCurve)
    {
        const float Progress = static_cast<float>(State.ShotCount - 1)
            + FMath::Clamp(SinceLastFire / FireInterval, 0.0f, 1.0f);
        State.OffsetDegrees = State.CurveAnchorDegrees + FVector2D(
            Source->GetRecoilPitchCurve().Eval(Progress), Source->GetRecoilYawCurve().Eval(Progress));
        if (!ensureMsgf(!State.OffsetDegrees.ContainsNaN(), TEXT("后坐力轨迹包含无效角度")))
        {
            State.OffsetDegrees = FVector2D::ZeroVector;
        }
    }
    else
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
    if (bAutomatic && !bHolding)
    {
        State.bStartNewSegment = true;
    }
    const FVector2D Limit = Context.AnimationConfig.RecoilLimitDegrees;
    const FVector2D Angles = State.OffsetDegrees * Scale;
    Facts.RecoilAngles.X = FMath::Clamp(Angles.X, -Limit.X, Limit.X);
    Facts.RecoilAngles.Y = FMath::Clamp(Angles.Y, -Limit.Y, Limit.Y);
}
