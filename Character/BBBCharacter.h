
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/BBBCharacterCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/BBBCharacterFacingSystem.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/BBBCharacterItemSystem.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
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
    
public:
    
    ABBBCharacter();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // ACharacter 提供的一个虚函数 用来绑定玩家输入
    virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;
    
    const FBBBCharacterConfig &GetCharacterConfig() const
    {
        return CharacterConfig;
    }
    
    const FBBBCharacterAnimationState &GetAnimationState() const
    {
        //动画只读取管线提交后的表现数据
        return RuntimeData.GetAnimationState();
    }
protected:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Config")
    FBBBCharacterConfig CharacterConfig;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Camera")
    TObjectPtr<UCameraComponent> FollowCamera;
    
    UPROPERTY(VisibleAnywhere, Category = "BBB|Network")
    TObjectPtr<UBBBCharacterNetworkComponent> CharacterNetworkComponent;
private:
    
    //表示黑板数据不参与持久化工作
    UPROPERTY(Transient)
    FBBBCharacterRuntimeData RuntimeData;
    
    FBBBCharacterExternalAPI CharacterExternalAPI;
    
    FBBBCharacterCameraSystem CameraSystem;
    
    FBBBCharacterAimSystem AimSystem;
    
    FBBBCharacterLocomotionSystem LocomotionSystem;
    
    FBBBCharacterFacingSystem FacingSystem;
    
    FBBBCharacterItemSystem ItemSystem;
    
    FBBBCharacterAnimationSystem AnimationSystem;
    
    FBBBCharacterNetworkSystem NetworkSystem;
    
    FBBBInputPipeline InputPipeline;
    
    FBBBIntentPipeline IntentPipeline;
    
    FBBBRequestPipeline RequestPipeline;
    
    FBBBArbitrationPipeline ArbitrationPipeline;
    
    FBBBExecutionPipeline ExecutionPipeline;
    
    FBBBCharacterUpdatePipeline CharacterUpdatePipeline;
};
