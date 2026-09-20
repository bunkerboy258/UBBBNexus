#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBCharacterMontageRequestState.h"

void FBBBCharacterInputProcessor::Update(
    FBBBCharacterRuntimeData &Data,
    UBBBEquipmentCatalog &Catalog) const
{
    FBBBCharacterParseState &State = Data.Operation;
    FBBBCharacterInputState &Input = Data.InputState;

    State.BeginFrame(Data.Equipment.Equipment.GetActiveMainHandInstance());
    FBBBCharacterMontageRequestState::BeginFrame(Data.Animation, State);

    FBBBCharacterInputContext Context{
        State,
        Data.Equipment.Inventory,
        Data.Equipment.Equipment,
        Data.Equipment.Commands,
        Data.Equipment.Events,
        Data.Animation,
        Data.Aim,
        Data.Locomotion,
        Data.CameraInput,
        Catalog};

    Input.BeginProcessing();

    // 还原输入最先建立远端角色的权威状态基座
    Process(Input.EquipmentState, Context);
    Process(Input.AimState, Context);
    Process(Input.LocomotionState, Context);

    // 已形成事实按玩法因果顺序驱动装备镜像和角色事件
    Process(Input.EquipFact, Context);
    Process(Input.FireFact, Context);
    Process(Input.ReloadStartedFact, Context);
    Process(Input.MagazineDetachedFact, Context);
    Process(Input.MagazineLoadedFact, Context);
    Process(Input.ReloadCancelledFact, Context);

    // 连续控制先覆盖本帧基座，本机客户端保留副本供网络命令处理器上传
    Process(Input.Movement, Context);
    Process(Input.Aim, Context);

    // 请求顺序就是冲突优先级，后续包直接观察前序包已经产生的解析状态
    Process(Input.EquipSlot, Context);
    Process(Input.Reload, Context);
    Process(Input.Fire, Context);
    Process(Input.Jump, Context);

    // 动画通知输入只推进已经存在的换弹操作，不参与请求竞争

    // 每个蒙太奇槽位独立覆盖，处理顺序与动画图中的层级保持一致
    Process(Input.FullBodyMontage, Context);
    Process(Input.UpperBodyMontage, Context);
    Process(Input.FullBodyAdditivePreAimMontage, Context);
    Process(Input.UpperBodyAdditiveMontage, Context);
    Process(Input.AdditiveHitReactMontage, Context);

    Process(Input.Camera, Context);

    Input.EndProcessing();

    // 权威实例与本机控制实例都需要发布控制，普通远端实例只应用网络还原状态
    FinalizeControl(Data);
}

void FBBBCharacterInputProcessor::FinalizeControl(FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterParseState &State = Data.Operation;
    State.Control.bFire = State.bFire;

    // 瞄准或开火时禁止冲刺
    State.Control.bSprint = State.Control.bSprint && !State.Control.bAim && !State.Control.bFire;
    Data.Control.Value = State.Control;
}
