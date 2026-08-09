
#include "BBBWork/UBBBNexus/Character/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/Results/BBBAimResults.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

void FBBBCharacterAimSystem::Initialize(
    APawn &InPawn,
    FBBBAimRuntimeData &InAimData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBAimConfig &InAimConfig)
{
    Pawn = &InPawn;
    AimData = &InAimData;
    IntentData = &InIntentData;
    EquipmentState = &InEquipmentState;
    AimConfig = &InAimConfig;
}

void FBBBCharacterAimSystem::Update()
{
    if (!ensureMsgf(Pawn && AimData && IntentData && EquipmentState && AimConfig && Pawn->GetWorld(), TEXT("[UBBBC]Aim system update failed because dependencies are null")))
    { return; }
    //复制上一帧状态
    FBBBAimRuntimeState State = AimData->GetState();
    //读取当前主手物品
    UBBBEquipmentInstance *ActiveInstance = EquipmentState->GetActiveMainHandInstance();
    //提交是否可瞄准
    State.bIsAiming = IntentData->WantsAim() || IntentData->WantsFire();
    
    FVector ViewLocation = FVector::ZeroVector;
    FRotator ViewRotation = FRotator::ZeroRotator;
    AController *Controller = Pawn->GetController();
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    /*输出两个值，都是世界空间*/
    FBBBAimTraceResult TraceResult;
    
    if (BuildAimTrace(
        ViewLocation,
        ViewRotation,
        //忽略当前装备实体避免命中自身武器
        ActiveInstance
            ? ActiveInstance->GetPresentationActor()
            : nullptr,
        //写入结果
        TraceResult))
    {
        State.AimTargetWorld = TraceResult.AimTarget;
    }
    //射线方向无效时 默认回退角色前方向
    if (!TraceResult.bValid)
    {
        State.AimTargetWorld = Pawn->GetActorLocation()
            + Pawn->GetActorForwardVector() * AimConfig->AimTraceDistance;
    }
    
    AimData->CommitLocalState(State);
}

bool FBBBCharacterAimSystem::BuildAimTrace(
    const FVector &ViewLocation,
    const FRotator &ViewRotation,
    AActor *EquippedActor,
    FBBBAimTraceResult &OutResult) const
{
    //把视点旋转转换为归一化世界方向
    const FVector ViewDirection = ViewRotation.Vector().GetSafeNormal();
    if (ViewDirection.IsNearlyZero())
    {
        return false;
    }
    //沿视线方向延伸到配置最大距离
    const FVector ViewEnd = ViewLocation + ViewDirection * AimConfig->AimTraceDistance;
    //创建复杂碰撞查询 并默认忽略角色
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BBBAimTrace), true, Pawn);
    //忽略角色自身全部碰撞
    QueryParams.AddIgnoredActor(Pawn);
    //避免射线命中自身武器
    if (EquippedActor)
    {
        //把当前装备加入忽略列表
        QueryParams.AddIgnoredActor(EquippedActor);
    }
    FHitResult Hit;
    //使用可见性通道执行单次射线检测
    const bool bHit = Pawn->GetWorld()->LineTraceSingleByChannel(
        Hit,
        ViewLocation,
        ViewEnd,
        ECC_Visibility,
        QueryParams);
    //方向有效即标记射线结果可用
    OutResult.bValid = true;
    //未命中时默认瞄准最大终点
    OutResult.AimTarget = ViewEnd;
    //命中可见物体时缩短目标到碰撞点
    if (bHit)
    {
        //使用真实表面冲击位置作为目标
        OutResult.AimTarget = Hit.ImpactPoint;
    }
    return true;
}
