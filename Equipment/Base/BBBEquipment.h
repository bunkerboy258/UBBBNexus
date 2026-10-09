#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipment.generated.h"

class UBBBEquipmentAnimInstance;
class UAnimInstance;
class UBBBEquipmentDefinition;
class UArrowComponent;
class USkeletalMeshComponent;
class FBBBEquipmentInitializer;
class FBBBEquipmentUpdatePipeline;
class UBBBEquipmentNetworkComponent;
struct FBBBEquipmentSecondaryLocalControlPacket;
struct FBBBEquipmentEquipLocalControlPacket;
struct FBBBEquipmentEquipAuthorityFactPacket;
struct FBBBEquipmentPrimaryLocalControlPacket;
struct FBBBEquipmentActionPermissionLocalControlPacket;
struct FBBBEquipmentBeginActionLocalControlPacket;
struct FBBBEquipmentEndActionLocalControlPacket;
struct FBBBEquipmentBeginContactLocalControlPacket;
struct FBBBEquipmentEndContactLocalControlPacket;


/** 单件装备的共享演员 配置和固定行为入口 */
UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API ABBBEquipment : public AActor
{
    GENERATED_BODY()

public:
    ABBBEquipment();

    /** 装备开始运行时初始化自身 @return 无 */
    virtual void BeginPlay() override;

    /** @return 装备配置标识 */
    FName GetEquipmentId() const;

    /** @return 装备静态配置 */
    UBBBEquipmentDefinition *GetDefinition() const
    {
        return Definition;
    }

    /** @return 装备自身是否完成初始化 */
    bool IsInitialized() const
    {
        return bInitialized;
    }

    /**
     * 读取装备挂接偏移
     * @param OutOffset	有效时写入挂接偏移
     * @return 是否具有有效配置
     */
    bool TryGetAttachmentOffset(FTransform &OutOffset) const;

    /** @return 角色应链接的动画层类型 */
    TSubclassOf<UAnimInstance> GetCharacterAnimationLayerClass() const;

    /** @return 持有角色当前是否只执行镜像还原 */
    bool IsMirror() const;

    /** @return 当前是否是持有者的激活装备 */
    bool IsEquipped() const;

    /** @return 装备骨骼网格 */
    USkeletalMeshComponent *GetEquipmentSkeletalMesh() const;

    /**
     * 尝试读取装备提供的枪口世界变换 不具备枪口时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const;

    /**
     * 读取供玩家界面展示的弹匣结果
     * @param Loaded	当前弹量
     * @param Capacity	弹匣容量
     * @param bContinuous	是否使用连续弹药弧
     * @return 装备是否提供弹匣显示
     */
    virtual bool ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const;

    /** @return 装备动画实例 */
    UBBBEquipmentAnimInstance *GetEquipmentAnimationInstance() const;

    /**
     * 提交装备输入或网络传输结果
     * @param Packet	待提交的数据
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("装备输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 装备自身的网络协议组件 */
    UBBBEquipmentNetworkComponent *GetNetworkComponent() const;

    /** @param EndPlayReason 结束原因 @return 无 */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

protected:
    /** 初始化具体装备状态 @return 是否初始化成功 */
    virtual bool InitializeRuntimeData() PURE_VIRTUAL(ABBBEquipment::InitializeRuntimeData, return false;);

    /** @return 无 通过具体关闭类收束自身 */
    virtual void ShutdownRuntimeData() PURE_VIRTUAL(ABBBEquipment::ShutdownRuntimeData, );

    /** @param Packet 通用次行为请求 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet);

    /** 由具体装备根创建自己的协议组件 */
    UPROPERTY(VisibleAnywhere, Category = "BBB|装备", meta = (DisplayName = "装备协议组件"))
    TObjectPtr<UBBBEquipmentNetworkComponent> NetworkComponent = nullptr;

    /** @param Packet	装备表现请求 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet);

    /** @param Packet	权威装备表现请求 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet);

    /** @param Packet	主行为请求 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet);

    /** @param Packet\t持有者操作许可 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet);

    /** @param Packet BeginAction动画事实 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentBeginActionLocalControlPacket Packet);

    /** @param Packet EndAction动画事实 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentEndActionLocalControlPacket Packet);

    /** @param Packet BeginContact动画事实 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentBeginContactLocalControlPacket Packet);

    /** @param Packet EndContact动画事实 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentEndContactLocalControlPacket Packet);

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备", meta = (AllowPrivateAccess = "true", DisplayName = "定义资产"))
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|装备", meta = (AllowPrivateAccess = "true", DisplayName = "装备根组件"))
    TObjectPtr<UArrowComponent> EquipmentRoot = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|装备", meta = (AllowPrivateAccess = "true", DisplayName = "装备骨骼网格组件"))
    TObjectPtr<USkeletalMeshComponent> EquipmentSkeletalMesh = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBBBEquipmentAnimInstance> EquipmentAnimationInstance = nullptr;

    /** 装备初始化结果 */
    UPROPERTY(Transient)
    bool bInitialized = false;
};
