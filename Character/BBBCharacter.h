
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputSubmit.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/AimController/BBBCharacterAimController.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/EquipmentController/BBBCharacterEquipmentController.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/LocomotionController/BBBCharacterLocomotionController.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/Core/Update/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "GameFramework/Character.h"
#include "BBBCharacter.generated.h"
class FBBBCharacterInitializer;
class FBBBCharacterShutdown;
class UBBBAnimInstance;
class UBBBCharacterNetworkComponent;
class ABBBPlayerCameraSystem;
class ABBBEquipment;

UCLASS()
class ABBB_EVAC_API ABBBCharacter : public ACharacter
{
    GENERATED_BODY()

    /** 允许初始化器装配私有运行时对象 */
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterShutdown;
    /** 允许主管线调度角色持有的子管线 */
    friend class FBBBCharacterUpdatePipeline;

    friend class ABBBPlayerCameraSystem;

    
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
     * 获取角色静态配置
     * @return 角色配置常量引用
     */
    const FBBBCharacterConfig &GetCharacterConfig() const
    {
        return CharacterConfig;
    }

    /**
     * 提交离散快照包到本帧输入队列
     * @param Packet	输入包
     * @return 是否接受输入
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        return BBBCharacterInput::Submit(RuntimeData.Parse.InputState, Forward<TPacket>(Packet));
    }

    /** @return 当前激活主手装备 */
    ABBBEquipment *GetActiveEquipment() const
    {
        return RuntimeData.Equipment.ReadEquipmentSelectionState().ActiveMainHandInstance;
    }

    /** 角色公开持有的唯一运行时聚合黑板 */
    UPROPERTY(Transient)
    FBBBCharacterRuntimeData RuntimeData;

protected:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ABBB|Config")
    FBBBCharacterConfig CharacterConfig;
    
    UPROPERTY(VisibleAnywhere, Category = "ABBB|Network")
    TObjectPtr<UBBBCharacterNetworkComponent> CharacterNetworkComponent;
    /*分类命名为ABBB是为了快点找到（bushi*/

private:
    FBBBCharacterAimController AimController;

    FBBBCharacterLocomotionController LocomotionController;
    
    FBBBCharacterEquipmentController EquipmentController;

    FBBBCharacterParseSystem ParseSystem;
    
    FBBBCharacterAnimationSystem AnimationSystem;
    
    FBBBCharacterNetworkSystem NetworkSystem;
    

    FBBBCharacterUpdatePipeline CharacterUpdatePipeline;
};
