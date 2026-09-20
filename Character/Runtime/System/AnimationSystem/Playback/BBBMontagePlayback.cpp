#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Playback/BBBMontagePlayback.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBMontagePlayback::Start(UAnimInstance &AnimInstance, UAnimMontage &Montage,
    const float PlayRate, const int32 InSequence, const uint64 InRevision, const bool bInReload)
{
    // 保存换弹播放上下文供后续通知回调校验
    Character = Cast<ABBBCharacter>(AnimInstance.GetOwningActor());
    Animation = &AnimInstance;
    PlayedMontage = &Montage;
    Sequence = InSequence;
    Revision = InRevision;
    bReload = bInReload;
    if (!ensureMsgf(Character.IsValid() && Sequence > 0, TEXT("[UBBBC]Reload playback context is invalid")))
    {
        bFinished = true;
        return;
    }

    // 播放失败时立即结束并报告失败结果
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
    // 保存实例编号避免其他蒙太奇通知误触发本次换弹
    MontageInstanceId = Instance->GetInstanceID();

    // 绑定结束和混出回调以覆盖正常完成和中断路径
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UBBBMontagePlayback::OnEnded);
    AnimInstance.Montage_SetEndDelegate(EndDelegate, &Montage);

    FOnMontageBlendingOutStarted BlendDelegate;
    BlendDelegate.BindUObject(this, &UBBBMontagePlayback::OnBlendingOut);
    AnimInstance.Montage_SetBlendingOutDelegate(BlendDelegate, &Montage);

    // 监听换弹开始和结束通知
    AnimInstance.OnPlayMontageNotifyBegin.AddDynamic(this, &UBBBMontagePlayback::OnNotify);
}

void UBBBMontagePlayback::OnNotify(const FName Name, const FBranchingPointNotifyPayload &Payload)
{
    // 忽略已结束的播放和不属于当前实例的通知
    if (!bReload || bFinished || Payload.MontageInstanceID != MontageInstanceId || !Character.IsValid())
    {
        return;
    }
    if (Name == TEXT("BBB.Reload.Start") && !bStartReported)
    {
        // 首次开始通知提交换弹开始事实
        bStartReported = true;
        Character->ReportReloadStartNotify(Sequence);
    }
    if (Name == TEXT("BBB.Reload.End") && !bEndReported)
    {
        // 结束通知必须建立在开始通知已经到达的前提上
        if (!ensureMsgf(bStartReported, TEXT("[UBBBC]Reload end notify arrived before start Sequence=%d"), Sequence))
        {
            Finish(true);
            return;
        }
        bEndReported = true;
        Character->ReportReloadEndNotify(Sequence, EBBBCharacterReloadEndReasonDefinition::Loaded);
    }
}

void UBBBMontagePlayback::OnBlendingOut(UAnimMontage *Montage, const bool bInterrupted)
{
    // 混出中断时提前收束播放状态
    if (bInterrupted)
    {
        Finish(true);
    }
}

void UBBBMontagePlayback::OnEnded(UAnimMontage *Montage, const bool bInterrupted)
{
    // 播放结束统一进入收束流程
    Finish(bInterrupted);
}

void UBBBMontagePlayback::Finish(const bool bInterrupted)
{
    // 收束操作只允许执行一次
    if (bFinished)
    {
        return;
    }
    bFinished = true;
    // 缺少结束通知时使用中断或播放失败补交结束事实
    if (bReload && !bEndReported && Character.IsValid())
    {
        Character->ReportReloadEndNotify(Sequence, bInterrupted
            ? EBBBCharacterReloadEndReasonDefinition::Interrupted : EBBBCharacterReloadEndReasonDefinition::PlaybackFailed);
    }
    Unbind();
}

void UBBBMontagePlayback::Unbind()
{
    // 播放结束后移除动态通知避免对象继续接收回调
    if (UAnimInstance *Instance = Animation.Get())
    {
        Instance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UBBBMontagePlayback::OnNotify);
    }
}

bool UBBBMontagePlayback::IsFinished()
{
    // 播放上下文失效时按中断收束
    if (!Animation.IsValid() || !Character.IsValid())
    {
        Finish(true);
    }
    if (Character.IsValid() && Animation.Get() != Character->GetMesh()->GetAnimInstance())
    {
        Finish(true);
    }
    // 实例消失且没有结束标记时视为播放失败
    if (!bFinished && Animation.IsValid() && !Animation->GetMontageInstanceForID(MontageInstanceId))
    {
        Finish(false);
    }
    return bFinished;
}

void UBBBMontagePlayback::Cancel(const int32 InSequence)
{
    // 只取消序号匹配且仍在播放的换弹实例
    if (Sequence != InSequence || bFinished)
    {
        return;
    }

    if (Animation.IsValid() && PlayedMontage.IsValid())
    {
        // 只停止仍然对应当前实例的蒙太奇
        FAnimMontageInstance *Active = Animation->GetActiveInstanceForMontage(PlayedMontage.Get());
        if (Active && Active->GetInstanceID() == MontageInstanceId)
        {
            Animation->Montage_Stop(0.1f, PlayedMontage.Get());
        }
    }
    Finish(true);
}

void UBBBMontagePlayback::Stop()
{
    Cancel(Sequence);
}
