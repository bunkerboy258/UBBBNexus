
#pragma once
#include "CoreMinimal.h"
class APawn;
class FBBBArbitrationPipeline;
class FBBBCharacterAimSystem;
class FBBBCharacterAnimationSystem;
class FBBBCharacterCameraSystem;
class FBBBCharacterEquipmentSystem;
class FBBBCharacterLocomotionSystem;
class FBBBCharacterNetworkSystem;
class FBBBCharacterInitializer;
class FBBBExecutionPipeline;
class FBBBInputPipeline;
class FBBBIntentPipeline;
class FBBBRequestPipeline;
struct FBBBCharacterRuntimeData;

class ABBB_EVAC_API FBBBCharacterUpdatePipeline final
{
public:
    
    /**
     * 按本地控制与权威状态分发角色帧更新
     */
    void Update() const;
    
private:
    
    friend class FBBBCharacterInitializer;

    /**
     * 注入角色运行时数据与各系统及子管线引用
     * @param InPawn	目标 Pawn
     * @param InRuntimeData	角色运行时数据
     * @param InCameraSystem	相机系统
     * @param InAimSystem	瞄准系统
     * @param InLocomotionSystem	移动系统
     * @param InEquipmentSystem	物品系统
     * @param InNetworkSystem	网络系统
     * @param InAnimationSystem	动画系统
     * @param InInputPipeline	输入管线
     * @param InIntentPipeline	意图管线
     * @param InRequestPipeline	请求管线
     * @param InArbitrationPipeline	仲裁管线
     * @param InExecutionPipeline	执行管线
     */
    void Initialize(
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
        FBBBExecutionPipeline &InExecutionPipeline);

    /**
     * 本地控制且权威时执行完整帧更新流程
     */
    void UpdateLocalAuthority() const;

    /**
     * 本地控制但不权威时执行自主代理帧更新流程
     */
    void UpdateLocalAutonomous() const;

    /**
     * 远端模拟且权威时执行服务器侧帧更新流程
     */
    void UpdateRemoteAuthority() const;

    /**
     * 远端模拟且不权威时执行模拟代理帧更新流程
     */
    void UpdateRemoteSimulated() const;

    APawn *Pawn = nullptr;

    FBBBCharacterRuntimeData *RuntimeData = nullptr;

    FBBBCharacterCameraSystem *CameraSystem = nullptr;

    FBBBCharacterAimSystem *AimSystem = nullptr;

    FBBBCharacterLocomotionSystem *LocomotionSystem = nullptr;

    FBBBCharacterEquipmentSystem *EquipmentSystem = nullptr;

    FBBBCharacterNetworkSystem *NetworkSystem = nullptr;

    FBBBCharacterAnimationSystem *AnimationSystem = nullptr;

    FBBBInputPipeline *InputPipeline = nullptr;

    FBBBIntentPipeline *IntentPipeline = nullptr;

    FBBBRequestPipeline *RequestPipeline = nullptr;

    FBBBArbitrationPipeline *ArbitrationPipeline = nullptr;

    FBBBExecutionPipeline *ExecutionPipeline = nullptr;
};
