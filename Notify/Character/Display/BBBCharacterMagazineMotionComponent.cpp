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
    const double Duration = CurrentTime - PreviousTime;
    if (PreviousTime >= 0.0 && Duration > UE_SMALL_NUMBER)
    {
        LinearVelocity = ((CurrentTransform.GetLocation() - PreviousTransform.GetLocation()) / Duration).GetClampedToMaxSize(1500.0);
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
