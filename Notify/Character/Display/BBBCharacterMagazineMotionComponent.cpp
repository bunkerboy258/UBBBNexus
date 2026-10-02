#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterMagazineMotionComponent.h"

#include "Engine/World.h"

UBBBCharacterMagazineMotionComponent::UBBBCharacterMagazineMotionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

void UBBBCharacterMagazineMotionComponent::SampleMotion()
{
    const UWorld *World = GetWorld();
    if (!World)
    {
        return;
    }

    // 附着世界位移已经包含角色移动 不再叠加角色速度或读取未模拟物理的手骨速度
    UpdateComponentToWorld();
    const FTransform CurrentTransform = GetComponentTransform();
    const double CurrentTime = World->GetTimeSeconds();
    if (!ensureMsgf(!CurrentTransform.ContainsNaN() && FMath::IsFinite(CurrentTime),
        TEXT("角色弹匣轨迹采样包含无效变换或时间 %s"), *GetName()))
    {
        return;
    }

    const double Duration = CurrentTime - PreviousTime;

    // 同一游戏时刻的重复调用不推进样本基准 防止位移与时间来自不同采样区间
    if (PreviousTime >= 0.0 && Duration <= UE_SMALL_NUMBER)
    {
        return;
    }

    // 使用通知区间内全部位置样本拟合世界运动趋势 在线累计均值和协方差避免保存逐帧数组
    // 中心化累计避免世界坐标和游戏时间较大时相减损失精度 最后一次短暂回摆不会单独决定释放方向
    ++MotionSampleCount;
    const double TimeDeltaFromMean = CurrentTime - MeanSampleTime;
    const FVector PositionDeltaFromMean = CurrentTransform.GetLocation() - MeanSamplePosition;
    MeanSampleTime += TimeDeltaFromMean / MotionSampleCount;
    MeanSamplePosition += PositionDeltaFromMean / MotionSampleCount;
    TimeVariance += TimeDeltaFromMean * (CurrentTime - MeanSampleTime);
    TimePositionCovariance += TimeDeltaFromMean * (CurrentTransform.GetLocation() - MeanSamplePosition);

    if (MotionSampleCount > 1 && TimeVariance > 0.0)
    {
        LinearVelocity = (TimePositionCovariance / TimeVariance).GetClampedToMaxSize(1500.0);
    }

    if (PreviousTime >= 0.0 && Duration > UE_SMALL_NUMBER)
    {
        FQuat DeltaRotation = CurrentTransform.GetRotation() * PreviousTransform.GetRotation().Inverse();
        DeltaRotation.Normalize();
        DeltaRotation.EnforceShortestArcWith(FQuat::Identity);
        FVector Axis = FVector::ZeroVector;
        double Angle = 0.0;
        DeltaRotation.ToAxisAndAngle(Axis, Angle);
        AngularVelocity = (Axis * (Angle / Duration)).GetClampedToMaxSize(30.0);
    }

    if (!ensureMsgf(!LinearVelocity.ContainsNaN() && !AngularVelocity.ContainsNaN(), TEXT("角色弹匣运动采样包含无效速度 %s"), *GetName()))
    {
        LinearVelocity = FVector::ZeroVector;
        AngularVelocity = FVector::ZeroVector;
    }

    PreviousTransform = CurrentTransform;
    PreviousTime = CurrentTime;
}

void UBBBCharacterMagazineMotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    SampleMotion();
}
