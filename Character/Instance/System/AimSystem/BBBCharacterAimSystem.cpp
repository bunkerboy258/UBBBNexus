#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/States/BBBAimStates.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterAimSystem::Initialize(
    APawn &InPawn,
    FBBBAimRuntimeData &InAimData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBAimConfig &InAimConfig)
{
    Pawn = &InPawn;
    AimData = &InAimData;
    IntentData = &InIntentData;
    AimConfig = &InAimConfig;
}

void FBBBCharacterAimSystem::Update()
{
    // 瞄准更新需要角色视角瞄准状态意图和配置全部有效
    if (!ensureMsgf(
        Pawn && AimData && IntentData && AimConfig,
        TEXT("[UBBBC]Aim system update failed because dependencies are null")))
    {
        return;
    }

    // 在局部状态中完成本帧计算后再统一提交
    FBBBAimRuntimeState State = AimData->GetState();

    // 先根据角色意图更新瞄准状态
    AimStateProcessor.Update(*IntentData, State);

    // 只有进入瞄准状态时才更新远处目标点
    if (State.bIsAiming)
    {
        AimTargetProcessor.Update(*Pawn, AimConfig->AimTargetDistance, State);
    }

    // 发布完整的本地瞄准状态供其他系统读取
    AimData->CommitLocalState(State);
}
