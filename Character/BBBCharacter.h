
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/BBBCharacterPhysicalPresentationSystem.h"
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
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/BBBCharacterItemSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/BBBCharacterTraversalSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/BBBCharacterLifeSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/BBBCharacterAppearanceSystem.h"
#include "BBBCharacter.generated.h"
class FBBBCharacterInitializer;
class FBBBCharacterShutdown;
class UBBBAnimInstance;
class UBBBCharacterNetworkComponent;
class UBBBEquipmentNetworkComponent;
class ABBBPlayerCameraSystem;
class ABBBEquipment;
class UMotionWarpingComponent;
class UBBBCharacterHitReactionComponent;
class UPhysicalAnimationComponent;

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
    /** @return 角色当前移动的世界空间速度 单位为厘米每秒 */
    FVector GetMovementVelocity() const;

    /** @return 当前角色已经成立的生命阶段 */
    UFUNCTION(BlueprintPure, Category = "BBB|生命", meta = (DisplayName = "当前生命阶段"))
    EBBBCharacterLifePhase GetLifePhase() const
    {
        return RuntimeData.Life.ReadLifeState().Phase;
    }

    /** @return 当前生命阶段的剩余生命 */
    UFUNCTION(BlueprintPure, Category = "BBB|生命", meta = (DisplayName = "当前生命"))
    float GetHealth() const
    {
        return RuntimeData.Life.ReadLifeState().Health;
    }

    /** @return 本轮倒地标识 */
    uint64 GetDownedRevision() const
    {
        return RuntimeData.Life.ReadLifeState().DownedRevision;
    }
    /** @return 当前是否正在帮扶 */
    UFUNCTION(BlueprintPure, Category = "BBB|救援", meta = (DisplayName = "正在帮扶"))
    bool IsRescueHelping() const
    {
        return RuntimeData.Life.ReadRescueState().bHelping;
    }
    /** @return 当前是否正在被救援 */
    UFUNCTION(BlueprintPure, Category = "BBB|救援", meta = (DisplayName = "正在被救援"))
    bool IsRescueReceiving() const
    {
        return RuntimeData.Life.ReadRescueState().bReceiving;
    }
    /** @return 当前救援关系中的另一角色 */
    UFUNCTION(BlueprintPure, Category = "BBB|救援", meta = (DisplayName = "救援伙伴"))
    APawn *GetRescuePartner() const
    {
        return RuntimeData.Life.ReadRescueState().Partner.Get();
    }
    /** @return 当前救援操作标识 */
    uint64 GetRescueOperationId() const
    {
        return RuntimeData.Life.ReadRescueState().OperationId;
    }
    /** @return 最近的合格救援目标 */
    UFUNCTION(BlueprintPure, Category = "BBB|救援", meta = (DisplayName = "救援目标"))
    APawn *GetRescueTarget() const
    {
        return RuntimeData.Life.ReadRescueCandidateState().Target.Get();
    }
    /** @return 当前空间是否允许恢复 */
    bool CanRecoverFromDowned() const
    {
        return RuntimeData.Life.ReadRescueCandidateState().bCanRecover;
    }
    /** @return 当前救援的表现进度 */
    UFUNCTION(BlueprintPure, Category = "BBB|救援", meta = (DisplayName = "救援进度"))
    float GetRescueProgress() const
    {
        return RuntimeData.Life.ReadRescueState().Progress;
    }
    /** @return 最近救援结束原因 */
    UFUNCTION(BlueprintPure, Category = "BBB|救援", meta = (DisplayName = "救援结束原因"))
    FName GetRescueEndReason() const
    {
        return RuntimeData.Life.ReadRescueState().EndReason;
    }

    /** @return 当前持有装备已经成立的使用许可 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备", meta = (DisplayName = "装备可使用"))
    bool IsEquipmentUsable() const
    {
        return RuntimeData.Equipment.ReadEquipmentUseState().bUsable;
    }

    /** @return 当前装备使用许可的修订号 */
    uint64 GetEquipmentUseRevision() const
    {
        return RuntimeData.Equipment.ReadEquipmentUseState().Revision;
    }
    
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
     * 引擎命中适配器 只转发伤害输入
     * @param DamageAmount		本次生命损失
     * @param DamageEvent		引擎命中数据
     * @param EventInstigator	来源控制者 允许环境来源为空
     * @param DamageCauser		直接命中来源
     * @return 接受的伤害值 输入被拒绝时返回零
     */
    virtual float TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent,
        AController *EventInstigator, AActor *DamageCauser) override;

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

    /** @param Packet 蓝图构造的全身蒙太奇输入 @return 是否接受输入 */
    UFUNCTION(BlueprintCallable, Category = "BBB|输入")
    bool SubmitInput(const FBBBFullBodyMontageLocalControlPacket &Packet);

    /** @return 当前激活主手装备 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备")
    ABBBEquipment *GetActiveEquipment() const
    {
        return RuntimeData.Equipment.ReadEquipmentSelectionState().ActiveMainHandInstance;
    }

    /** @return 角色是否只执行网络镜像还原 */
    bool IsNetworkMirror() const;

    /** @return 角色是否拥有网络权威 */
    bool HasNetworkAuthority() const;

    /** @return 当前实际持有实例标识 */
    uint64 GetEquipmentGeneration() const
    {
        return RuntimeData.Equipment.ReadEquipmentSelectionState().ActiveGeneration;
    }

    /** @return 装备独立网络组件 */
    UBBBEquipmentNetworkComponent *GetEquipmentNetworkComponent() const;
    /** @return 角色所属的独立网络传输组件 */
    UBBBCharacterNetworkComponent *GetCharacterNetworkComponent() const
    {
        return CharacterNetworkComponent;
    }

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
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ABBB|配置", meta = (DisplayName = "角色配置资产"))
    TObjectPtr<UBBBCharacterConfig> CharacterConfigAsset = nullptr;
    
    UPROPERTY(VisibleAnywhere, Category = "ABBB|网络", meta = (DisplayName = "角色网络组件"))
    TObjectPtr<UBBBCharacterNetworkComponent> CharacterNetworkComponent;
    /** 装备独立网络传输组件 */
    UPROPERTY(VisibleAnywhere, Category = "ABBB|网络", meta = (DisplayName = "装备网络组件"))
    TObjectPtr<UBBBEquipmentNetworkComponent> EquipmentNetworkComponent;
    /*分类命名为ABBB是为了快点找到(bushi*/

private:
    /** 与角色网格绑定的局部物理受击组件 */
    UPROPERTY(VisibleAnywhere, Category = "BBB|物理", meta = (DisplayName = "物理受击"))
    TObjectPtr<UBBBCharacterHitReactionComponent> HitReaction;
    /** 局部受击的姿势恢复组件 */
    UPROPERTY(VisibleAnywhere, Category = "BBB|物理", meta = (DisplayName = "物理动画"))
    TObjectPtr<UPhysicalAnimationComponent> PhysicalAnimation;
    /** 角色物理表现调度器 */
    FBBBCharacterPhysicalPresentationSystem PhysicalPresentationSystem;
    /** 官方角色根运动校正组件 */
    UPROPERTY(VisibleAnywhere, Category = "BBB|翻越")
    TObjectPtr<UMotionWarpingComponent> MotionWarping;

    FBBBCharacterAimSystem AimSystem;

    FBBBCharacterLocomotionSystem LocomotionSystem;

    /** 无独立 Tick 的攀爬业务系统 */
    FBBBCharacterTraversalSystem TraversalSystem;

    /** 独立维护生命阶段和受击事实 */
    FBBBCharacterLifeSystem LifeSystem;
    
    FBBBCharacterItemSystem ItemSystem;

    FBBBCharacterAppearanceSystem AppearanceSystem;

    FBBBCharacterEquipmentSystem EquipmentSystem;

    FBBBCharacterParseSystem ParseSystem;
    
    FBBBCharacterAnimationSystem AnimationSystem;
    
    FBBBCharacterNetworkSystem NetworkSystem;
    

    FBBBCharacterUpdatePipeline CharacterUpdatePipeline;
};
