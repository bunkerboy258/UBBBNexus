#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Playback/BBBReloadMontagePlayback.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBReloadMontagePlayback::Start(UAnimInstance &AnimInstance, UAnimMontage &Montage,
    const float PlayRate, const int32 InSequence)
{
    Character = Cast<ABBBCharacterInstance>(AnimInstance.GetOwningActor());
    Animation = &AnimInstance;
    PlayedMontage = &Montage;
    Sequence = InSequence;
    if (!ensureMsgf(Character.IsValid() && Sequence > 0, TEXT("[UBBBC]Reload playback context is invalid")))
    {
        bFinished = true;
        return;
    }

    if (AnimInstance.Montage_Play(&Montage, PlayRate) <= 0.0f)
    {
        Finish(false);
        return;
    }

    FAnimMontageInstance *Instance = AnimInstance.GetActiveInstanceForMontage(&Montage);
    if (!ensureMsgf(Instance, TEXT("[UBBBC]Reload montage instance is missing")))
    {
        Finish(false);
        return;
    }
    MontageInstanceId = Instance->GetInstanceID();

    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UBBBReloadMontagePlayback::OnEnded);
    AnimInstance.Montage_SetEndDelegate(EndDelegate, &Montage);

    FOnMontageBlendingOutStarted BlendDelegate;
    BlendDelegate.BindUObject(this, &UBBBReloadMontagePlayback::OnBlendingOut);
    AnimInstance.Montage_SetBlendingOutDelegate(BlendDelegate, &Montage);

    AnimInstance.OnPlayMontageNotifyBegin.AddDynamic(this, &UBBBReloadMontagePlayback::OnNotify);
}

void UBBBReloadMontagePlayback::OnNotify(const FName Name, const FBranchingPointNotifyPayload &Payload)
{
    if (bFinished || Payload.MontageInstanceID != MontageInstanceId || !Character.IsValid())
    {
        return;
    }
    if (Name == TEXT("BBB.Reload.Start") && !bStartReported)
    {
        bStartReported = true;
        Character->ReportReloadStartNotify(Sequence);
    }
    if (Name == TEXT("BBB.Reload.End") && !bEndReported)
    {
        if (!ensureMsgf(bStartReported, TEXT("[UBBBC]Reload end notify arrived before start Sequence=%d"), Sequence))
        {
            Finish(true);
            return;
        }
        bEndReported = true;
        Character->ReportReloadEndNotify(Sequence, EBBBCharacterReloadEndReason::Loaded);
    }
}

void UBBBReloadMontagePlayback::OnBlendingOut(UAnimMontage *Montage, const bool bInterrupted)
{
    if (bInterrupted)
    {
        Finish(true);
    }
}

void UBBBReloadMontagePlayback::OnEnded(UAnimMontage *Montage, const bool bInterrupted)
{
    Finish(bInterrupted);
}

void UBBBReloadMontagePlayback::Finish(const bool bInterrupted)
{
    if (bFinished)
    {
        return;
    }
    bFinished = true;
    if (!bEndReported && Character.IsValid())
    {
        Character->ReportReloadEndNotify(Sequence, bInterrupted
            ? EBBBCharacterReloadEndReason::Interrupted : EBBBCharacterReloadEndReason::PlaybackFailed);
    }
    Unbind();
}

void UBBBReloadMontagePlayback::Unbind()
{
    if (UAnimInstance *Instance = Animation.Get())
    {
        Instance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UBBBReloadMontagePlayback::OnNotify);
    }
}

bool UBBBReloadMontagePlayback::IsFinished()
{
    if (!Animation.IsValid() || !Character.IsValid())
    {
        Finish(true);
    }
    if (Character.IsValid() && Animation.Get() != Character->GetMesh()->GetAnimInstance())
    {
        Finish(true);
    }
    if (!bFinished && Animation.IsValid() && !Animation->GetMontageInstanceForID(MontageInstanceId))
    {
        Finish(false);
    }
    return bFinished;
}

void UBBBReloadMontagePlayback::Cancel(const int32 InSequence)
{
    if (Sequence != InSequence || bFinished)
    {
        return;
    }

    if (Animation.IsValid() && PlayedMontage.IsValid())
    {
        FAnimMontageInstance *Active = Animation->GetActiveInstanceForMontage(PlayedMontage.Get());
        if (Active && Active->GetInstanceID() == MontageInstanceId)
        {
            Animation->Montage_Stop(0.1f, PlayedMontage.Get());
        }
    }
    Finish(true);
}
