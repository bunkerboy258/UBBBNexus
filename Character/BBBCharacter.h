
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/BBBCharacterCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/BBBCharacterFacingSystem.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdateTickFunction.h"
#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/BBBExecutionPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/BBBInputPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/BBBIntentPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/BBBRequestPipeline.h"
#include "BBBWork/UBBBNexus/Character/Runtime/BBBCharacterRuntimeData.h"
#include "GameFramework/Character.h"
#include "BBBCharacter.generated.h"
class FBBBCharacterInitializer;
class UBBBAnimInstance;
class UBBBCharacterNetworkComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ABBB_EVAC_API ABBBCharacter : public ACharacter
{
    GENERATED_BODY()

    /** 允许初始化器装配私有运行时对象 */
    friend class FBBBCharacterInitializer;
    /** 允许主管线调度角色持有的子管线 */
    friend class FBBBCharacterUpdatePipeline;
    /** 允许移动后更新函数调用角色LateUpdate */
    friend struct FBBBCharacterLateUpdateTickFunction;

    /** 允许动画实例只读角色表现状态 */
    friend class UBBBAnimInstance;

    
public:
    
    /**
     * 构造角色并装配相机臂与相机等默认组件
     */
    ABBBCharacter();
    /**
     * 游戏开始时通过初始化器装配全部运行时对象
     */
    virtual void BeginPlay() override;
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

private:

    /** 在移动组件完成本帧移动后驱动主管线LateUpdate */
    void LateUpdate();

    /**
     * 获取动画表现状态
     * @return 管线提交后的动画状态常量引用
     */
    const FBBBCharacterAnimationState &GetAnimationState() const
    {
        //动画只读取管线提交后的表现数据
        return RuntimeData.GetAnimationState();
    }

    /** @return 瞄准系统提交的只读状态 */
    const FBBBAimRuntimeState &GetAimState() const
    {
        return RuntimeData.Aim.GetState();
    }
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
    
    FBBBCharacterExternalAPI CharacterExternalAPI;
    
    FBBBCharacterCameraSystem CameraSystem;
    
    FBBBCharacterAimSystem AimSystem;

    /** 角色身体朝向系统 */
    FBBBCharacterFacingSystem FacingSystem;
    
    FBBBCharacterLocomotionSystem LocomotionSystem;
    
    FBBBCharacterEquipmentSystem EquipmentSystem;
    
    FBBBCharacterAnimationSystem AnimationSystem;
    
    FBBBCharacterNetworkSystem NetworkSystem;
    
    FBBBInputPipeline InputPipeline;
    
    FBBBIntentPipeline IntentPipeline;
    
    FBBBRequestPipeline RequestPipeline;
    
    FBBBArbitrationPipeline ArbitrationPipeline;
    
    FBBBExecutionPipeline ExecutionPipeline;

    /** 驱动角色移动后更新阶段的独立更新函数 */
    FBBBCharacterLateUpdateTickFunction LateUpdateTickFunction;
    
    FBBBCharacterUpdatePipeline CharacterUpdatePipeline;
};
