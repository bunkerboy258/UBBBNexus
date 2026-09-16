
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/BBBCharacterCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdate.h"
#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/BBBExecutionPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/BBBInputPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/BBBIntentPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/BBBRequestPipeline.h"
#include "BBBWork/UBBBNexus/Character/Runtime/BBBCharacterRuntimeData.h"
#include "GameFramework/Character.h"
#include "BBBCharacterInstance.generated.h"
class FBBBCharacterInitializer;
class UBBBAnimInstance;
class UBBBCharacterNetworkComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ABBB_EVAC_API ABBBCharacterInstance : public ACharacter
{
    GENERATED_BODY()

    /** 允许初始化器装配私有运行时对象 */
    friend class FBBBCharacterInitializer;
    /** 允许主管线调度角色持有的子管线 */
    friend class FBBBCharacterUpdatePipeline;
    /** 允许移动后更新函数调用角色LateUpdate */
    friend struct FBBBCharacterLateUpdate;

    /** 允许动画实例只读角色表现状态 */
    friend class UBBBAnimInstance;

    
public:
    
    /**
     * 构造角色并装配相机臂与相机等默认组件
     */
    ABBBCharacterInstance();
    /**
     * 游戏开始时通过初始化器装配全部运行时对象
     */
    virtual void BeginPlay() override;

    /**
     * 游戏结束时停止角色移动后更新
     * @param EndPlayReason 角色停止游戏的原因
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /**
     * 每帧驱动角色主更新管线
     * @param DeltaSeconds	帧间隔秒数
     */
    virtual void Tick(float DeltaSeconds) override;

    /**
     * 注册角色主管线与移动后更新函数
     * @param bRegister 是否注册更新函数
     */
    virtual void RegisterActorTickFunctions(bool bRegister) override;

    /** @return 始终返回true以向模拟代理复制真实移动加速度 */
    virtual bool ShouldReplicateAcceleration() const override;

    /**
     * 绑定玩家输入到输入管线
     * @param PlayerInputComponent	玩家输入组件
     */
    virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;

    /**
     * 获取角色静态配置
     * @return 角色配置常量引用
     */
    const FBBBCharacterConfig &GetCharacterConfig() const
    {
        return CharacterConfig;
    }

    /** @return 装备向角色提交表现贡献的入口 */
    FBBBCharacterExternalAPI &GetExternalAPI()
    {
        return ExternalAPI;
    }

private:

    /** 在移动组件完成本帧移动后驱动主管线LateUpdate */
    void LateUpdate();

protected:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ABBB|Config")
    FBBBCharacterConfig CharacterConfig;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ABBB|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ABBB|Camera")
    TObjectPtr<UCameraComponent> FollowCamera;
    
    UPROPERTY(VisibleAnywhere, Category = "ABBB|Network")
    TObjectPtr<UBBBCharacterNetworkComponent> CharacterNetworkComponent;
    /*分类命名为ABBB是为了快点找到（bushi*/

private:
    
    //表示黑板数据不参与持久化工作
    UPROPERTY(Transient)
    FBBBCharacterRuntimeData RuntimeData;
    
    FBBBCharacterCameraSystem CameraSystem;
    
    FBBBCharacterAimSystem AimSystem;

    FBBBCharacterLocomotionSystem LocomotionSystem;
    
    FBBBCharacterEquipmentSystem EquipmentSystem;

    /** 接收装备提交的角色表现贡献 */
    FBBBCharacterExternalAPI ExternalAPI;
    
    FBBBCharacterAnimationSystem AnimationSystem;
    
    FBBBCharacterNetworkSystem NetworkSystem;
    
    FBBBInputPipeline InputPipeline;
    
    FBBBIntentPipeline IntentPipeline;
    
    FBBBRequestPipeline RequestPipeline;
    
    FBBBArbitrationPipeline ArbitrationPipeline;
    
    FBBBExecutionPipeline ExecutionPipeline;

    /** 驱动角色移动后更新阶段的独立更新函数 */
    FBBBCharacterLateUpdate LateUpdateTick;
    
    FBBBCharacterUpdatePipeline CharacterUpdatePipeline;
};
