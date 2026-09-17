
#include "BBBWork/UBBBNexus/Character/Instance/Core/Update/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/CameraSystem/BBBCharacterCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Execution/BBBExecutionPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/BBBInputPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/BBBIntentPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/BBBRequestPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterUpdatePipeline::Initialize(
    APawn &InPawn,
    FBBBCharacterRuntimeData &InRuntimeData,
    FBBBCharacterCameraSystem &InCameraSystem,
    FBBBCharacterAimSystem &InAimSystem,
    FBBBCharacterLocomotionSystem &InLocomotionSystem,
    FBBBCharacterEquipmentSystem &InEquipmentSystem,
    FBBBCharacterNetworkSystem &InNetworkSystem,
    FBBBCharacterAnimationSystem &InAnimationSystem,
    FBBBInputPipeline &InInputPipeline,
    FBBBIntentPipeline &InIntentPipeline,
    FBBBRequestPipeline &InRequestPipeline,
    FBBBArbitrationPipeline &InArbitrationPipeline,
    FBBBExecutionPipeline &InExecutionPipeline)
{
    Pawn = &InPawn;
    RuntimeData = &InRuntimeData;
    CameraSystem = &InCameraSystem;
    AimSystem = &InAimSystem;
    LocomotionSystem = &InLocomotionSystem;
    EquipmentSystem = &InEquipmentSystem;
    NetworkSystem = &InNetworkSystem;
    AnimationSystem = &InAnimationSystem;

    //保存角色持有的子管线供主管线按固定顺序调度
    InputPipeline = &InInputPipeline;
    IntentPipeline = &InIntentPipeline;
    RequestPipeline = &InRequestPipeline;
    ArbitrationPipeline = &InArbitrationPipeline;
    ExecutionPipeline = &InExecutionPipeline;
}

void FBBBCharacterUpdatePipeline::Update() const
{
    // 总更新必须在全部系统注入完成后执行
    if (!ensureMsgf(
        Pawn
            && RuntimeData
            && CameraSystem
            && AimSystem
            && LocomotionSystem
            && EquipmentSystem
            && NetworkSystem
            && AnimationSystem
            && InputPipeline
            && IntentPipeline
            && RequestPipeline
            && ArbitrationPipeline
            && ExecutionPipeline,
        TEXT("[UBBBC]Pipeline update aborted because injected systems are null")))
    { return; }

    // 根据角色网络身份选择唯一更新分支
    //本地控制？
    const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

    //权威？
    const bool bHasAuthority = Pawn->HasAuthority();

    //本地控制且权威
    if (bIsLocallyControlled && bHasAuthority)
    {
        UpdateLocalAuthority();
        return;
    }

    //本地控制但不权威
    if (bIsLocallyControlled && !bHasAuthority)
    {
        UpdateLocalAutonomous();
        return;
    }

    //远端模拟但权威
    if (!bIsLocallyControlled && bHasAuthority)
    {
        UpdateRemoteAuthority();
        return;
    }

    //远端模拟且不权威
    if (!bIsLocallyControlled && !bHasAuthority)
    {
        UpdateRemoteSimulated();
        return;
    }
    
}

//------------------------------------------------------------------------------

void FBBBCharacterUpdatePipeline::LateUpdate() const
{
    // 延迟更新只处理动画事实和帧数据清理
    if (!ensureMsgf(
        RuntimeData && AnimationSystem && EquipmentSystem,
        TEXT("[UBBBC]Pipeline LateUpdate aborted because injected dependencies are null")))
    {
        return;
    }

    //移动组件完成本帧位置与旋转后再生成最终动画事实
    AnimationSystem->Update();

    //所有本帧消费者执行完成后统一清理角色黑板
    RuntimeData->Clean();
}

//本地控制且权威
void FBBBCharacterUpdatePipeline::UpdateLocalAuthority() const
{
    // 本地权威先恢复网络状态再执行本地请求链路
    NetworkSystem->UpdateRestore();
    
    InputPipeline->Update();
    
    IntentPipeline->Update();
    
    RequestPipeline->Update();
    
    ArbitrationPipeline->Update();
    
    ExecutionPipeline->Update();
    
    EquipmentSystem->Update();
    
    CameraSystem->Update();

    AimSystem->Update();

    LocomotionSystem->Update();
    
    // 本地完整状态更新结束后上传可复制数据
    NetworkSystem->UpdateUpload();
}

//本地控制但不权威
void FBBBCharacterUpdatePipeline::UpdateLocalAutonomous() const
{
    // 本地自主使用本地输入链路并同步可复制结果
    NetworkSystem->UpdateRestore();
    
    InputPipeline->Update();
    
    IntentPipeline->Update();
    
    RequestPipeline->Update();
    
    ArbitrationPipeline->Update();
    
    ExecutionPipeline->Update();
    
    EquipmentSystem->Update();
    
    CameraSystem->Update();

    AimSystem->Update();

    LocomotionSystem->Update();
    
    NetworkSystem->UpdateUpload();
}

//远端模拟且权威
void FBBBCharacterUpdatePipeline::UpdateRemoteAuthority() const
{
    // 远端权威只恢复网络状态并驱动装备更新
    NetworkSystem->UpdateRestore();

    EquipmentSystem->Update();
}

//远端模拟且不权威
void FBBBCharacterUpdatePipeline::UpdateRemoteSimulated() const
{
    // 远端模拟只消费网络恢复结果不生成本地请求
    NetworkSystem->UpdateRestore();

    EquipmentSystem->Update();
}
