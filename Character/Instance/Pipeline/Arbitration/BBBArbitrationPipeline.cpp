#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include <concepts>

namespace
{
    template <typename TPacket>
    void ApplyRules(const TArray<TPacket> &Packets, FBBBCharacterRuntimeData &Data)
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
}

void FBBBArbitrationPipeline::Update(const bool bRestoreMode) const
{
    if (!ensureMsgf(Data, TEXT("[UBBBC]Arbitration pipeline is uninitialized")))
    {
        return;
    }

    FBBBInputBatch &Frame = Data->Input.Frame;
    Data->Operation.bRestoreMode = bRestoreMode;
    FBBBCharacterReloadInput::BeginFrame(*Data);

    ApplyRules(Frame.Results, *Data);

    // 镜像仅还原权威端结果 不解析本地玩家控制和动画通知
    if (bRestoreMode)
    {
        ApplyRules(Frame.Restores, *Data);
    }
    else
    {
        if (ensureMsgf(Data->Input.FrameControl.CanApply(*Data), TEXT("[UBBBC]Invalid frame control")))
        {
            Data->Input.FrameControl.Apply(*Data);
        }
        ApplyRules(Frame.Equip, *Data);
        FBBBCharacterEquipInput::FinishFrame(*Data);
        ApplyRules(Frame.Notifications, *Data);
        ApplyRules(Frame.Reload, *Data);

        // 持续开火与离散请求共用同一条固定规则 同帧换弹先于开火
        FBBBCharacterFireInput::ApplyHeld(*Data);
        ApplyRules(Frame.Fire, *Data);
        FBBBCharacterControlInput::FinishFrame(*Data);

        FBBBPlayerCameraInput::BeginFrame(*Data);
        ApplyRules(Frame.Camera, *Data);
    }

    FBBBCharacterMontagePacket::BeginFrame(*Data);
    ApplyRules(Frame.Montages, *Data);
}
