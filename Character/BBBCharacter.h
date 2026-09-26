
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputSubmit.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
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
class UBBBEquipmentNetworkComponent;
class ABBBPlayerCameraSystem;
class ABBBEquipment;

enum class EBBBCharacterMontageSlot : uint8
{
    /** 未知槽位 */
    Unknown,
    /** 全身槽位 */
    FullBody,
    /** 上半身槽位 */
    UpperBody,
    /** 瞄准前全身叠加槽位 */
    FullBodyAdditivePreAim,
    /** 上半身叠加槽位 */
    UpperBodyAdditive,
    /** 受击叠加槽位 */
    AdditiveHitReact
};

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
    const UBBBCharacterConfig &GetCharacterConfig() const
    {
        checkf(
            IsValid(CharacterConfigAsset),
            TEXT("角色 %s 缺少 UBBBCharacterConfig 资产"),
            *GetPathName());

        return *CharacterConfigAsset;
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

    /** @return 角色是否只执行网络镜像还原 */
    bool IsNetworkMirror() const;

    /** @return 角色是否拥有网络权威 */
    bool HasNetworkAuthority() const;

    /** @return 装备独立网络组件 */
    UBBBEquipmentNetworkComponent *GetEquipmentNetworkComponent() const;

    /**
     * 读取右手骨骼世界变换
     * @param OutTransform	有效时写入右手骨骼世界变换
     * @return 是否成功读取
     */
    bool TryGetRightHandWorldTransform(FTransform &OutTransform) const;

    /**
     * 将公开槽位名称映射到角色动画入口
     * @param SlotName	槽位名称
     * @return 已知槽位类别 未知时返回 Unknown
     */
    static EBBBCharacterMontageSlot ClassifyMontageSlot(FName SlotName);

    /** 角色公开持有的唯一运行时聚合黑板 */
    UPROPERTY(Transient)
    FBBBCharacterRuntimeData RuntimeData;

protected:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ABBB|Config")
    TObjectPtr<UBBBCharacterConfig> CharacterConfigAsset = nullptr;
    
    UPROPERTY(VisibleAnywhere, Category = "ABBB|Network")
    TObjectPtr<UBBBCharacterNetworkComponent> CharacterNetworkComponent;
    /** 装备独立网络传输组件 */
    UPROPERTY(VisibleAnywhere, Category = "ABBB|Network")
    TObjectPtr<UBBBEquipmentNetworkComponent> EquipmentNetworkComponent;
    /*分类命名为ABBB是为了快点找到（bushi*/

private:
    FBBBCharacterAimSystem AimSystem;

    FBBBCharacterLocomotionSystem LocomotionSystem;
    
    FBBBCharacterEquipmentSystem EquipmentSystem;

    FBBBCharacterParseSystem ParseSystem;
    
    FBBBCharacterAnimationSystem AnimationSystem;
    
    FBBBCharacterNetworkSystem NetworkSystem;
    

    FBBBCharacterUpdatePipeline CharacterUpdatePipeline;
};
