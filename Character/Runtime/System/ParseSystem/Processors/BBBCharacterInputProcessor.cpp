#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterPacketContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageRequest.h"

void FBBBCharacterInputProcessor::Update(
    FBBBCharacterRuntimeData &Data,
    UBBBEquipmentCatalog &Catalog,
    const bool bAuthority,
    const bool bLocallyControlled) const
{
    FBBBCharacterParseState &State = Data.Operation;
    FBBBCharacterInputFrame &Input = Data.InputFrame;
    const bool bRestoreMode = !bAuthority;
    const bool bPreserveForNetwork = !bAuthority && bLocallyControlled;

    State.BeginFrame(bRestoreMode, Data.Equipment.Equipment.GetActiveMainHandInstance());
    FBBBCharacterMontagePacket::BeginFrame(Data.Animation, State);

    FBBBCharacterPacketContext Context{
        State,
        Data.Equipment.Inventory,
        Data.Equipment.Equipment,
        Data.Equipment.Commands,
        Data.Equipment.Events,
        Data.Animation,
        Data.Aim,
        Data.Locomotion,
        Data.CameraInput,
        Catalog,
        bAuthority,
        bLocallyControlled};

    Input.BeginProcessing();

    // 还原输入最先建立远端角色的权威状态基座
    Process(Input.RestoreEquipment, Context);
    Process(Input.RestoreAim, Context);
    Process(Input.RestoreLocomotion, Context);

    // 已形成事实按玩法因果顺序驱动装备镜像和角色事件
    Process(Input.EquipFact, Context);
    Process(Input.FireFact, Context);
    Process(Input.ReloadStartedFact, Context);
    Process(Input.MagazineDetachedFact, Context);
    Process(Input.MagazineLoadedFact, Context);
    Process(Input.ReloadCancelledFact, Context);

    // 连续控制先覆盖本帧基座，本机客户端保留副本供网络命令处理器上传
    ProcessNetworkCommand(Input.Movement, Context, bPreserveForNetwork);
    ProcessNetworkCommand(Input.Aim, Context, bPreserveForNetwork);

    // 请求顺序就是冲突优先级，后续包直接观察前序包已经产生的解析状态
    ProcessNetworkCommand(Input.EquipSlot, Context, bPreserveForNetwork);
    ProcessNetworkCommand(Input.Reload, Context, bPreserveForNetwork);
    ProcessNetworkCommand(Input.Fire, Context, bPreserveForNetwork);
    Process(Input.Jump, Context);

    // 动画通知输入只推进已经存在的换弹操作，不参与请求竞争
    Process(Input.ReloadDetach, Context);
    Process(Input.ReloadLoad, Context);
    Process(Input.ReloadInterrupt, Context);

    // 每个蒙太奇槽位独立覆盖，处理顺序与动画图中的层级保持一致
    Process(Input.FullBodyMontage, Context);
    Process(Input.UpperBodyMontage, Context);
    Process(Input.FullBodyAdditivePreAimMontage, Context);
    Process(Input.UpperBodyAdditiveMontage, Context);
    Process(Input.AdditiveHitReactMontage, Context);

    Process(Input.Camera, Context);

    Input.EndProcessing();

    // 权威实例与本机控制实例都需要发布控制，普通远端实例只应用网络还原状态
    if (bAuthority || bLocallyControlled)
    {
        FinalizeControl(Data);
    }
}

void FBBBCharacterInputProcessor::FinalizeControl(FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterParseState &State = Data.Operation;
    State.Control.bFire = State.bFire;

    // 瞄准或开火时禁止冲刺
    State.Control.bSprint = State.Control.bSprint && !State.Control.bAim && !State.Control.bFire;
    Data.Control.Value = State.Control;
}
