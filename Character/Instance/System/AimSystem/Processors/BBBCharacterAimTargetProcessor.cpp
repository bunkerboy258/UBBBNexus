#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Processors/BBBCharacterAimTargetProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/States/BBBAimStates.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterAimTargetProcessor::Update(
    APawn &Pawn,
    float AimTargetDistance,
    FBBBAimRuntimeState &State) const
{
    // 瞄准目标使用控制器视点而不是角色朝向
    AController *Controller = Pawn.GetController();
    if (!ensureMsgf(Controller, TEXT("[UBBBC]Aim target update failed because controller is null")))
    {
        return;
    }

    FVector ViewLocation = FVector::ZeroVector;
    FRotator ViewRotation = FRotator::ZeroRotator;
    // 读取实际玩家视点保证目标与镜头方向一致
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

    // 视线归一化后才能按配置距离投射目标
    const FVector ViewDirection = ViewRotation.Vector().GetSafeNormal();
    if (!ensureMsgf(!ViewDirection.IsNearlyZero(), TEXT("[UBBBC]Aim target update failed because view direction is invalid")))
    {
        return;
    }

    // 沿视线生成远处瞄准目标供表现系统使用
    State.AimTargetWorld = ViewLocation + ViewDirection * FMath::Max(AimTargetDistance, 0.0f);
}
