#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/BBBCharacterAimController.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/DomainData/Context/BBBAimUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/DomainData/BBBAimDomainState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/DomainData/States/BBBAimState.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterAimController::Initialize(
    FBBBAimDomainState &InAimData,
    const FBBBCharacterControlState &InIntentData)
{
    AimData = &InAimData;
    ControlData = &InIntentData;
}

void FBBBCharacterAimController::Update()
{
    // 瞄准更新需要角色视角瞄准状态意图和配置全部有效
    if (!AimData || !ControlData)
    {
        return;
    }

    // 在局部状态中完成本帧计算后再统一提交
    FBBBAimUpdateContext Context{AimData->AimState, *ControlData};

    // 先根据角色意图更新瞄准状态
    AimStateProcessor.Update(Context.ControlState, Context.AimState);

    // 只有进入瞄准状态时才更新远处目标点
    if (Context.AimState.bIsAiming)
    {
        AimTargetProcessor.Update(Context.ControlState, Context.AimState);
    }

}
