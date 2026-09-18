#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/BBBCharacterAimController.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/States/BBBAimStates.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterAimController::Initialize(
    FBBBAimRuntimeData &InAimData,
    const FBBBCharacterControlState &InIntentData)
{
    AimData = &InAimData;
    ControlData = &InIntentData;
}

void FBBBCharacterAimController::Update()
{
    // 瞄准更新需要角色视角瞄准状态意图和配置全部有效
    if (!ensureMsgf(
        AimData && ControlData,
        TEXT("[UBBBC]Aim system update failed because dependencies are null")))
    {
        return;
    }

    // 在局部状态中完成本帧计算后再统一提交
    FBBBAimRuntimeState State = AimData->GetState();

    // 先根据角色意图更新瞄准状态
    AimStateProcessor.Update(*ControlData, State);

    // 只有进入瞄准状态时才更新远处目标点
    if (State.bIsAiming)
    {
        AimTargetProcessor.Update(*ControlData, State);
    }

    // 发布完整的本地瞄准状态供其他系统读取
    AimData->CommitLocalState(State);
}
