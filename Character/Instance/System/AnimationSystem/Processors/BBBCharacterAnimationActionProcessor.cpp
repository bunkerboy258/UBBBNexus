#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/Instance/ExternalAPI/Packets/BBBCharacterMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Playback/BBBReloadMontagePlayback.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"

void FBBBCharacterAnimationActionProcessor::Update(
    UBBBAnimInstance &AnimInstance,
    FBBBAnimationRuntimeData &AnimationData) const
{
    // 先取消已经标记的换弹播放实例
    for (const int32 Sequence : AnimationData.CancelledReloadSequences)
    {
        for (UBBBReloadMontagePlayback *Playback : AnimationData.ReloadPlaybacks)
        {
            if (Playback)
            {
                Playback->Cancel(Sequence);
            }
        }
    }
    AnimationData.CancelledReloadSequences.Reset();

    // 移除无效或已经结束的换弹播放对象
    AnimationData.ReloadPlaybacks.RemoveAll([](const TObjectPtr<UBBBReloadMontagePlayback> &Playback)
    {
        return !Playback || Playback->IsFinished();
    });

    // 取出当前动画队列并清空原队列避免重复播放
    TArray<FBBBCharacterMontagePacket> Packets = MoveTemp(AnimationData.MontageQueue);
    AnimationData.MontageQueue.Reset();

    for (const FBBBCharacterMontagePacket &Packet : Packets)
    {
        // 无效动画请求不能进入播放系统
        if (!ensureMsgf(
            Packet.Montage && FMath::IsFinite(Packet.PlayRate) && Packet.PlayRate > 0.0f,
            TEXT("[UBBBC]Queued equipment montage packet is invalid")))
        {
            continue;
        }

        if (Packet.bReload)
        {
            // 换弹动画使用独立播放对象接收关键帧通知
            UBBBReloadMontagePlayback *Playback = NewObject<UBBBReloadMontagePlayback>(&AnimInstance);
            AnimationData.ReloadPlaybacks.Add(Playback);
            Playback->Start(AnimInstance, *Packet.Montage, Packet.PlayRate, Packet.Sequence);
            continue;
        }

        // 普通装备动作直接交给角色动画实例播放
        AnimInstance.ExecuteEquipmentActionMontage(
            Packet.Montage,
            Packet.PlayRate);
    }
}
