
#include "BBBWork/UBBBNexus/Character/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/Results/BBBAimResults.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/States/BBBCharacterItemStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"

void FBBBCharacterAimSystem::Initialize(
    APawn &InPawn,
    USkeletalMeshComponent &InCharacterMesh,
    FBBBAimRuntimeData &InAimData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterAnimationCommands &InAnimationCommands,
    const FBBBAimConfig &InAimConfig,
    const FBBBAimAnimationConfig &InAnimationConfig)
{
    Pawn = &InPawn;
    CharacterMesh = &InCharacterMesh;
    AimData = &InAimData;
    WorldData = &InWorldData;
    IntentData = &InIntentData;
    EquipmentState = &InEquipmentState;
    AnimationCommands = &InAnimationCommands;
    AimConfig = &InAimConfig;
    AnimationConfig = &InAnimationConfig;
}

void FBBBCharacterAimSystem::Update()
{
    if (!ensureMsgf(Pawn && CharacterMesh && WorldData && AimData && IntentData && EquipmentState && AnimationCommands && AimConfig && AnimationConfig && Pawn->GetWorld(), TEXT("[UBBBC]Aim system update failed because dependencies are null")))
    { return; }
    //复制上一帧状态
    FBBBAimRuntimeState State = AimData->GetState();
    //读取当前主手物品
    UBBBEquipmentInstance *ActiveInstance = EquipmentState->GetActiveMainHandInstance();
    const UBBBEquipmentDefinition *ActiveDefinition = ActiveInstance
        ? ActiveInstance->GetEquipmentDefinition()
        : nullptr;
    const bool bCanAim = ActiveDefinition
        && IntentData->WantsAim()
        //装备切换动画期间禁止物品IK
        && !AnimationCommands->IsItemIKBlockedRequested();
    //提交是否可瞄准
    State.bIsAiming = bCanAim;
    
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
            ? ActiveInstance->GetModelActor()
            : nullptr,
        //写入结果
        TraceResult))
    {
        State.AimTargetWorld = TraceResult.AimTarget;
        State.AimTraceStart = TraceResult.ViewStart;
        State.AimTraceEnd = TraceResult.ViewEnd;
    }
    //射线方向无效时 默认回退角色前方向
    if (!TraceResult.bValid)
    {
        State.AimTargetWorld = Pawn->GetActorLocation()
            + Pawn->GetActorForwardVector() * AimConfig->AimTraceDistance;
    }
    
    State.AimIKTargetWorld = State.AimTargetWorld;
    
    //插槽无效时的默认回退位置
    FVector AimOriginWorld = Pawn->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
    if (ensureMsgf(
        !AimConfig->AimOriginSocketName.IsNone()
            && CharacterMesh->DoesSocketExist(AimConfig->AimOriginSocketName),
        TEXT("[UBBBC]Aim origin socket is not configured or missing on character mesh")))
    {
        AimOriginWorld = CharacterMesh->GetSocketLocation(AimConfig->AimOriginSocketName);
    }
    //角色自身为基准建立一套坐标系
    const FMatrix ReferenceMatrix = FRotationMatrix::MakeFromXZ(
        //水平参考轴 使用角色前方向
        Pawn->GetActorForwardVector(),
        //垂直参考轴 使用角色上方向
        Pawn->GetActorUpVector());
    //计算瞄准单位世界方向
    const FVector AimDirection = (FVector(State.AimIKTargetWorld) - AimOriginWorld).GetSafeNormal();
    //转换到角色参考空间
    const FVector LocalAimDirection = ReferenceMatrix.InverseTransformVector(AimDirection);
    //转换为水平瞄准角
    const FRotator AimRotation = LocalAimDirection.Rotation();
    //补偿并限制水平瞄准角到有效范围
    const float TargetYaw = FMath::Clamp(
        AimRotation.Yaw * AnimationConfig->AimYawCompensation,
        -90.0f,
        90.0f);
    const float DeltaSeconds = WorldData->GetFrameDeltaSeconds();
    //平滑水平角避免动画姿态跳变
    State.AimYaw = FMath::FInterpTo(
        State.AimYaw,
        TargetYaw,
        DeltaSeconds,
        AnimationConfig->AimInterpSpeed);

    AimData->CommitLocalState(State, AimOriginWorld);
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
    //写入本次检测视点起点
    OutResult.ViewStart = ViewLocation;
    //写入本次检测单位方向
    OutResult.ViewDirection = ViewDirection;
    //写入本次检测最大终点
    OutResult.ViewEnd = ViewEnd;
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
