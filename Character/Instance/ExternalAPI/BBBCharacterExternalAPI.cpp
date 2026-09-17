#include "BBBWork/UBBBNexus/Character/Instance/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterExternalAPI::Initialize(ABBBCharacter &Character, FBBBAnimationRuntimeData &Animation,
    FBBBInputRuntimeData &Input, FBBBCharacterEquipmentEvents &Events,
    FBBBCharacterEquipmentState &EquipmentState)
{
    // 保存角色外部接口需要访问的固定运行数据
    Owner = &Character;
    AnimationData = &Animation;
    InputData = &Input;
    EquipmentEvents = &Events;
    CharacterEquipmentState = &EquipmentState;
}

bool FBBBCharacterExternalAPI::SubmitEquipmentMontage(UAnimMontage *Montage, const float PlayRate,
    const int32 Sequence, const bool bReload)
{
    // 外部装备只能在游戏线程提交有效的蒙太奇请求
    if (!ensureMsgf(IsInGameThread() && AnimationData && Montage
        && FMath::IsFinite(PlayRate) && PlayRate > 0.0f, TEXT("[UBBBC]Invalid montage contribution")))
    {
        return false;
    }

    // 将装备动画封装后交给角色动画系统统一消费
    FBBBCharacterMontagePacket Packet;
    Packet.Montage = Montage;
    Packet.PlayRate = PlayRate;
    Packet.Sequence = Sequence;
    Packet.bReload = bReload;
    AnimationData->MontageQueue.Add(MoveTemp(Packet));
    return true;
}

void FBBBCharacterExternalAPI::SubmitReloadInput(const int32 Sequence, const EBBBCharacterReloadAnimationPhase Phase)
{
    // 换弹通知必须在依赖有效时进入输入管线
    if (!ensureMsgf(IsInGameThread() && Owner && InputData, TEXT("[UBBBC]Reload input dependencies are invalid")))
    {
        return;
    }

    // 远程角色不产生本地换弹输入
    if (!Owner->IsLocallyControlled())
    {
        return;
    }

    // 操作序号用于关联对应的装备动作
    if (!ensureMsgf(Sequence > 0, TEXT("[UBBBC]Reload animation input has no action sequence")))
    {
        return;
    }

    // 将动画通知放入待处理队列避免直接修改装备状态
    FBBBCharacterReloadAnimationInput Input;
    Input.Sequence = Sequence;
    Input.Phase = Phase;
    InputData->PendingReloadInputs.Add(Input);
}

void FBBBCharacterExternalAPI::SubmitReloadStartNotify(const int32 Sequence)
{
    SubmitReloadInput(Sequence, EBBBCharacterReloadAnimationPhase::Start);
}

void FBBBCharacterExternalAPI::SubmitReloadEndNotify(const int32 Sequence, const EBBBCharacterReloadEndReason EndReason)
{
    // 播放失败时保留告警帮助定位动画结束通知缺失
    if (EndReason == EBBBCharacterReloadEndReason::PlaybackFailed)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBBBC]Reload montage failed or missed its end notify Sequence=%d"), Sequence);
    }
    // 只有完成装填才结束换弹其余情况统一视为中断
    SubmitReloadInput(Sequence, EndReason == EBBBCharacterReloadEndReason::Loaded
        ? EBBBCharacterReloadAnimationPhase::End : EBBBCharacterReloadAnimationPhase::Interrupted);
}

void FBBBCharacterExternalAPI::PublishEquipmentEvent(const FBBBEquipmentActionEvent &Event)
{
    if (ensureMsgf(IsInGameThread() && EquipmentEvents && CharacterEquipmentState,
        TEXT("[UBBBC]Equipment result sink is unavailable")))
    {
        // 先写入装备事实再同步当前换弹序号
        EquipmentEvents->AddAction(Event);
        ABBBEquipment *ActiveEquipment = CharacterEquipmentState->GetActiveMainHandInstance();
        // 只有当前主手装备的事件才能推进换弹生命周期
        if (ActiveEquipment && Event.EquipmentId == ActiveEquipment->GetEquipmentId())
        {
            if (Event.Phase == EBBBCharacterEquipmentPhase::ReloadStarted)
            {
                CharacterEquipmentState->ReloadSequence = Event.Sequence;
            }
            if ((Event.Phase == EBBBCharacterEquipmentPhase::MagazineLoaded
                || Event.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
                && Event.Sequence == CharacterEquipmentState->ReloadSequence)
            {
                CharacterEquipmentState->ReloadSequence = INDEX_NONE;
            }
        }

        // 取消换弹时移除未播放动画并记录过期序号
        if (Event.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled && AnimationData)
        {
            AnimationData->MontageQueue.RemoveAll([&Event](const FBBBCharacterMontagePacket &Packet)
            {
                return Packet.bReload && Packet.Sequence == Event.Sequence;
            });
            AnimationData->CancelledReloadSequences.AddUnique(Event.Sequence);
        }
    }
}

void FBBBCharacterExternalAPI::SubmitEquipmentRecoil(const FVector2D Impulse, const float RecoverySpeed)
{
    // 后坐力事件只接受游戏线程上的有效数值
    if (!ensureMsgf(IsInGameThread() && EquipmentEvents && !Impulse.ContainsNaN()
        && FMath::IsFinite(RecoverySpeed) && RecoverySpeed > 0.0f, TEXT("[UBBBC]Invalid recoil contribution")))
    {
        return;
    }

    // 将后坐力作为瞬时事件交给装备事件流处理
    FBBBEquipmentRecoilEvent Event;
    Event.Impulse = Impulse;
    Event.RecoverySpeed = RecoverySpeed;
    EquipmentEvents->AddRecoil(Event);
}
