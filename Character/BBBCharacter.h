
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputSubmit.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterReloadEndReason.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/BBBCharacterAimController.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/BBBCharacterEquipmentController.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/BBBCharacterLocomotionController.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdate.h"
#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/BBBCharacterRuntimeData.h"
#include "GameFramework/Character.h"
#include "BBBCharacter.generated.h"
class FBBBCharacterInitializer;
class FBBBCharacterShutdown;
class UBBBAnimInstance;
class UBBBCharacterNetworkComponent;
class ABBBPlayerCameraSystem;

UCLASS()
class ABBB_EVAC_API ABBBCharacter : public ACharacter
{
    GENERATED_BODY()

    /** 允许初始化器装配私有运行时对象 */
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterShutdown;
    /** 允许主管线调度角色持有的子管线 */
    friend class FBBBCharacterUpdatePipeline;
    /** 允许移动后更新函数调用角色LateUpdate */
    friend struct FBBBCharacterLateUpdate;

    /** 允许动画实例只读角色表现状态 */
    friend class UBBBAnimInstance;
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
        return BBBCharacterInput::Submit(RuntimeData, Forward<TPacket>(Packet));
    }

    /**
     * 接收换弹卸下关键帧
     * @param Sequence	播放时保存的操作序号
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Animation Input")
    void ReportReloadStartNotify(int32 Sequence);

    /**
     * 接收换弹装填或中断事件
     * @param Sequence	播放时保存的操作序号
     * @param EndReason	结束原因
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Animation Input")
    void ReportReloadEndNotify(int32 Sequence, EBBBCharacterReloadEndReason EndReason);

private:

    /** 在移动组件完成本帧移动后驱动主管线LateUpdate */
    void LateUpdate();

protected:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ABBB|Config")
    FBBBCharacterConfig CharacterConfig;
    
    UPROPERTY(VisibleAnywhere, Category = "ABBB|Network")
    TObjectPtr<UBBBCharacterNetworkComponent> CharacterNetworkComponent;
    /*分类命名为ABBB是为了快点找到（bushi*/

private:
    
    //表示黑板数据不参与持久化工作
    UPROPERTY(Transient)
    FBBBCharacterRuntimeData RuntimeData;
    
    
    FBBBCharacterAimController AimController;

    FBBBCharacterLocomotionController LocomotionController;
    
    FBBBCharacterEquipmentController EquipmentController;

    FBBBCharacterParseSystem ParseSystem;
    
    FBBBCharacterAnimationSystem AnimationSystem;
    
    FBBBCharacterNetworkSystem NetworkSystem;
    

    /** 驱动角色移动后更新阶段的独立更新函数 */
    FBBBCharacterLateUpdate LateUpdateTick;
    
    FBBBCharacterUpdatePipeline CharacterUpdatePipeline;
};
