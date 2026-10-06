#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterAnimInstance.h"

#include "Animation/AnimSequenceBase.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Actor.h"

void UBBBMonsterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    AActor* const Owner = GetOwningActor();
    Presentation = Owner ? Owner->FindComponentByClass<UBBBMonsterPresentationComponent>() : nullptr;
    bHasSnapshot = false;
    bChannelBActive = false;
    ChannelAAnimation = nullptr;
    ChannelBAnimation = nullptr;
    ChannelATime = 0.0f;
    ChannelBTime = 0.0f;
}

void UBBBMonsterAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    const UBBBMonsterPresentationComponent* const Source = Presentation.Get();
    if (!Source || !Source->bHasAppliedAnimation)
    {
        return;
    }

    const bool bFirstSnapshot = !bHasSnapshot;
    const bool bNewAction = bFirstSnapshot || LastBehavior != Source->BBBMonsterBehavior || LastActionId != Source->LastActionId || LastStateEnteredTime != Source->StateEnteredTime;
    TransitionBlendTime = FMath::Clamp(Source->AnimationBlendTime, 0.0f, 0.5f);

    if (bNewAction)
    {
        UAnimSequenceBase* const Animation = Source->SelectAnimationForAction(Source->BBBMonsterBehavior, Source->LastActionId, GetTypeHash(GetOwningActor()->GetFName()));
        const USkeletalMeshComponent* const Mesh = GetSkelMeshComponent();
        const USkeletalMesh* const MeshAsset = Mesh ? Mesh->GetSkeletalMeshAsset() : nullptr;
        if (!ensureMsgf(Animation && MeshAsset && Animation->GetSkeleton() == MeshAsset->GetSkeleton() && Animation->GetPlayLength() > 0.0f,
            TEXT("[UBBBM]Animation snapshot requires a positive-length sequence on the exact presentation skeleton")))
        {
            return;
        }

        if (!bHasSnapshot)
        {
            ChannelAAnimation = Animation;
            ChannelBAnimation = Animation;
        }

        if (bHasSnapshot)
        {
            bChannelBActive = !bChannelBActive;
        }

        TObjectPtr<UAnimSequenceBase>& ActiveAnimation = bChannelBActive ? ChannelBAnimation : ChannelAAnimation;
        ActiveAnimation = Animation;
        float& ActiveTime = bChannelBActive ? ChannelBTime : ChannelATime;
        ActiveTime = 0.0f;
        LastBehavior = Source->BBBMonsterBehavior;
        LastActionId = Source->LastActionId;
        LastStateEnteredTime = Source->StateEnteredTime;
        bHasSnapshot = true;
    }

    UAnimSequenceBase* const Animation = GetActiveAnimation();
    float& ActiveTime = bChannelBActive ? ChannelBTime : ChannelATime;
    const float Length = Animation->GetPlayLength();
    const bool bLooping = LastBehavior == EBBBMonsterBehavior::Idle || LastBehavior == EBBBMonsterBehavior::Alert || LastBehavior == EBBBMonsterBehavior::Patrol || LastBehavior == EBBBMonsterBehavior::Chase;

    if (bLooping)
    {
        float Rate = 1.0f;
        if (LastBehavior == EBBBMonsterBehavior::Patrol || LastBehavior == EBBBMonsterBehavior::Chase)
        {
            Rate = FMath::Clamp(Source->MovementSpeed / FMath::Max(Source->AnimationReferenceSpeed, 1.0f), 0.0f, 2.0f);
        }

        if (bNewAction)
        {
            ActiveTime = Length * static_cast<float>(GetTypeHash(GetOwningActor()->GetFName()) % 997) / 997.0f;
        }

        ActiveTime = FMath::Fmod(ActiveTime + FMath::Max(DeltaSeconds, 0.0f) * Rate, Length);
    }

    if (!bLooping)
    {
        ActiveTime = Length * Source->ActionProgress;
    }

    if (bFirstSnapshot)
    {
        ChannelATime = ActiveTime;
        ChannelBTime = ActiveTime;
    }
}

UAnimSequenceBase* UBBBMonsterAnimInstance::GetChannelAAnimation() const
{
    return ChannelAAnimation;
}

UAnimSequenceBase* UBBBMonsterAnimInstance::GetChannelBAnimation() const
{
    return ChannelBAnimation;
}

float UBBBMonsterAnimInstance::GetChannelATime() const
{
    return ChannelATime;
}

float UBBBMonsterAnimInstance::GetChannelBTime() const
{
    return ChannelBTime;
}

bool UBBBMonsterAnimInstance::IsChannelBActive() const
{
    return bChannelBActive;
}

float UBBBMonsterAnimInstance::GetTransitionBlendTime() const
{
    return TransitionBlendTime;
}

UAnimSequenceBase* UBBBMonsterAnimInstance::GetActiveAnimation() const
{
    return bChannelBActive ? ChannelBAnimation : ChannelAAnimation;
}

float UBBBMonsterAnimInstance::GetActiveAnimationTime() const
{
    return bChannelBActive ? ChannelBTime : ChannelATime;
}
