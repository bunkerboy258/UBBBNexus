#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

namespace
{
    template <typename TPacket>
    void ApplyContribution(const TArray<TPacket> &Packets, FBBBCharacterRuntimeData &Data)
    {
        for (const TPacket &Packet : Packets)
        {
            if (Packet.CanApply(Data))
            {
                Packet.Apply(Data);
            }
        }
    }
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
        if (!ensureMsgf(Result.Sequence > 0, TEXT("[UBBBC]Invalid equipment feedback")))
        {
            continue;
        }
        Data->Equipment.Events.AddAction(Result);
        FBBBReloadBehavior::OnEquipmentResult(Result, *Data);
    }

    // 镜像只还原已完成因果 不重新执行本地行为规则
    if (bRestoreMode)
    {
        ApplyContribution(Frame.Restores, *Data);
    }
    else
    {
        Data->Operation.Control = Data->Input.FrameControl;

        // 顺序即冲突规则 切枪优先于换弹 换弹优先于开火
        for (const FBBBEquipInput &Input : Frame.Equip)
        {
            if (FBBBEquipBehavior::CanStart(Input, *Data))
            {
                FBBBEquipBehavior::Start(Input, *Data);
            }
        }
        for (const FBBBCharacterReloadAnimationInput &Event : Frame.Notifications)
        {
            FBBBReloadBehavior::OnAnimationEvent(Event, *Data);
        }
        for (const FBBBReloadInput &Input : Frame.Reload)
        {
            if (FBBBReloadBehavior::CanStart(Input, *Data))
            {
                FBBBReloadBehavior::Start(Input, *Data);
            }
        }

        const bool bHeldFire = Data->Operation.Control.bFire;
        Data->Operation.Control.bFire = false;
        if (bHeldFire && FBBBFireBehavior::CanStart(FBBBFireInput(), *Data))
        {
            FBBBFireBehavior::Start(FBBBFireInput(), *Data);
        }
        for (const FBBBFireInput &Input : Frame.Fire)
        {
            if (!Data->Operation.bFire && FBBBFireBehavior::CanStart(Input, *Data))
            {
                FBBBFireBehavior::Start(Input, *Data);
            }
        }
        Data->Operation.Control.bSprint = Data->Operation.Control.bSprint
            && FBBBCharacterControlRule::AllowsSprint(Data->Operation.Control.bAim,
                Data->Operation.Control.bFire);
        Data->Control.Value = Data->Operation.Control;
        Data->CameraContributions = Frame.Camera;
    }

    FBBBCharacterMontagePacket::BeginFrame(*Data);
    ApplyContribution(Frame.Montages, *Data);
}
