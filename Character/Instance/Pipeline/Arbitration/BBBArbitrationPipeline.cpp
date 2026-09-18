#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBBehaviorContract.h"

class FBBBBehaviorRuleDispatcher final
{
public:
    template <CBBBBehavior TBehavior>
    static void Process(const typename TBehavior::FInput &Input, FBBBCharacterRuntimeData &Data)
    {
        if (TBehavior::Policy.IsBlocked(Data.Operation.ActiveBehaviorGroups)
            || !TBehavior::CanStart(Input, Data))
        {
            return;
        }

        // 取消语义只负责调度 具体撤销事实由所属行为完成
        if ((TBehavior::Policy.Cancels & BBBBehaviorGroup::Reload) != 0
            && (Data.Operation.ActiveBehaviorGroups & BBBBehaviorGroup::Reload) != 0)
        {
            FBBBReloadBehavior::Cancel(Data);
        }
        TBehavior::Start(Input, Data);
    }

    template <CBBBBehavior TBehavior>
    static void ProcessRequests(const TArray<typename TBehavior::FInput> &Inputs,
        FBBBCharacterRuntimeData &Data)
    {
        static_assert(TBehavior::Policy.Delivery == EBBBBehaviorDelivery::Request);
        for (const typename TBehavior::FInput &Input : Inputs)
        {
            Process<TBehavior>(Input, Data);
        }
    }

    template <CBBBBehavior TBehavior>
    static void ProcessSnapshot(const typename TBehavior::FInput &Input, FBBBCharacterRuntimeData &Data)
    {
        static_assert(TBehavior::Policy.Delivery == EBBBBehaviorDelivery::Snapshot);
        Process<TBehavior>(Input, Data);
    }
};

namespace
{
    template <typename TPacket>
    void ApplyContribution(const TArray<TPacket> &Packets, FBBBCharacterRuntimeData &Data)
    {
        static_assert(requires(const TPacket &Packet, FBBBCharacterRuntimeData &Runtime)
        {
            { Packet.CanApply(Runtime) } -> std::convertible_to<bool>;
            Packet.Apply(Runtime);
        });
        for (const TPacket &Packet : Packets)
        {
            if (Packet.CanApply(Data))
            {
                Packet.Apply(Data);
            }
        }
    }

    static_assert(FBBBEquipBehavior::Policy.Priority > FBBBReloadBehavior::Policy.Priority);
    static_assert(FBBBReloadBehavior::Policy.Priority > FBBBFireBehavior::Policy.Priority);
    static_assert(FBBBFireBehavior::Policy.Priority > FBBBSprintBehavior::Policy.Priority);
}

void FBBBArbitrationPipeline::Update(const bool bRestoreMode) const
{
    if (!ensureMsgf(Data, TEXT("[UBBBC]Arbitration pipeline is uninitialized")))
    {
        return;
    }

    FBBBInputBatch &Frame = Data->Input.Frame;
    Data->Operation.bRestoreMode = bRestoreMode;
    FBBBReloadBehavior::BeginFrame(*Data);

    // 装备已确认结果先记录为事实 再交给持有跨帧状态的行为处理
    for (const FBBBEquipmentActionEvent &Result : Frame.Results)
    {
        if (!ensureMsgf(Result.IsValid(), TEXT("[UBBBC]Invalid equipment feedback")))
        {
            continue;
        }
        Result.Record(*Data);
        FBBBReloadBehavior::OnEquipmentResult(Result, *Data);
    }

    // 镜像只还原已完成因果 不重新执行本地行为规则
    if (bRestoreMode)
    {
        ApplyContribution(Frame.Restores, *Data);
    }
    else
    {
        Data->Operation.Control = FBBBCharacterControlFacts();
        const FBBBContinuousInputState &Control = Data->Input.FrameContinuous;
        FBBBBehaviorRuleDispatcher::ProcessSnapshot<FBBBMoveBehavior>(Control.Move, *Data);
        FBBBBehaviorRuleDispatcher::ProcessSnapshot<FBBBViewBehavior>(Control.View, *Data);
        FBBBBehaviorRuleDispatcher::ProcessSnapshot<FBBBAimBehavior>(Control.Aim, *Data);
        FBBBBehaviorRuleDispatcher::ProcessSnapshot<FBBBWalkBehavior>(Control.Walk, *Data);
        FBBBBehaviorRuleDispatcher::ProcessSnapshot<FBBBCrouchBehavior>(Control.Crouch, *Data);
        FBBBBehaviorRuleDispatcher::ProcessRequests<FBBBJumpBehavior>(Frame.Jump, *Data);

        FBBBBehaviorRuleDispatcher::ProcessRequests<FBBBEquipBehavior>(Frame.Equip, *Data);
        for (const FBBBCharacterReloadAnimationInput &Event : Frame.Notifications)
        {
            FBBBReloadBehavior::OnAnimationEvent(Event, *Data);
        }
        FBBBBehaviorRuleDispatcher::ProcessRequests<FBBBReloadBehavior>(Frame.Reload, *Data);
        FBBBBehaviorRuleDispatcher::ProcessRequests<FBBBFireBehavior>(Frame.Fire, *Data);
        FBBBBehaviorRuleDispatcher::ProcessSnapshot<FBBBSprintBehavior>(Control.Sprint, *Data);
        Data->Control.Value = Data->Operation.Control;

        FBBBPlayerCameraInput::BeginFrame(*Data);
        ApplyContribution(Frame.Camera, *Data);
    }

    FBBBCharacterMontagePacket::BeginFrame(*Data);
    ApplyContribution(Frame.Montages, *Data);
}
