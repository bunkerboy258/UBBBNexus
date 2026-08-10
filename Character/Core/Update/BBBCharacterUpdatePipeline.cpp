
#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/BBBCharacterCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/BBBExecutionPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/BBBInputPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/BBBIntentPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/BBBRequestPipeline.h"
#include "BBBWork/UBBBNexus/Character/Runtime/BBBCharacterRuntimeData.h"
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

//本地控制且权威
void FBBBCharacterUpdatePipeline::UpdateLocalAuthority() const
{
    NetworkSystem->UpdateValidation();
    
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

    AnimationSystem->Update();

    RuntimeData->Clean();
}

//本地控制但不权威
void FBBBCharacterUpdatePipeline::UpdateLocalAutonomous() const
{
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

    AnimationSystem->Update();

    RuntimeData->Clean();
}

//远端模拟且权威
void FBBBCharacterUpdatePipeline::UpdateRemoteAuthority() const
{
    NetworkSystem->UpdateValidation();
    
    NetworkSystem->UpdateRestore();
    
    EquipmentSystem->Update();

    AnimationSystem->Update();

    RuntimeData->Clean();
}

//远端模拟且不权威
void FBBBCharacterUpdatePipeline::UpdateRemoteSimulated() const
{
    NetworkSystem->UpdateRestore();
    
    EquipmentSystem->Update();

    AnimationSystem->Update();

    RuntimeData->Clean();
}
