#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/RemoteMessage/Equipment/FBBBEquipmentVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentVisibilityAuthorityFactPacket.h"
#include "BBBEquipment.generated.h"

class UBBBEquipmentAnimInstance;
class UAnimInstance;
class UBBBEquipmentDefinition;
class UArrowComponent;
class USkeletalMeshComponent;
class FBBBEquipmentInitializer;
class FBBBEquipmentUpdatePipeline;
class UBBBEquipmentNetworkComponent;
/** 单件装备的共享演员 配置和固定行为入口 */
UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API ABBBEquipment : public AActor
{
    GENERATED_BODY()

public:
    /** @return 网格隐藏事实 */
    virtual bool IsMeshHidden() const PURE_VIRTUAL(ABBBEquipment::IsMeshHidden, return false;);
    ABBBEquipment();

    /** @return 当前装备是否允许主动切换或解除装备 */
    virtual bool CanSwitch() const;

    /** @return 当前装备是否允许快捷消耗品临时打断 */
    virtual bool CanQuickUse() const;

    /** @return 装备继承的世界空间移动速度 单位为厘米每秒 */
    virtual FVector GetVelocity() const override;

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
     * @return 无 仅贡献输入
     */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 装备自身的网络协议组件 */
    UBBBEquipmentNetworkComponent *GetNetworkComponent() const;

    /** @param EndPlayReason 结束原因 @return 无 */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

protected:
    virtual void QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );
    virtual void QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );
    virtual void QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );
    virtual void QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );
    /** @param Packet 由具体装备解释的阻止开火请求 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** @param Packet 由具体装备解释的允许开火请求 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** @param Packet 由具体装备解释的装入弹药请求 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** @param Packet 由具体装备解释的中断换弹请求 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** 初始化具体装备状态 @return 是否初始化成功 */
    virtual bool InitializeRuntimeData() PURE_VIRTUAL(ABBBEquipment::InitializeRuntimeData, return false;);

    /** @return 无 通过具体关闭类收束自身 */
    virtual void ShutdownRuntimeData() PURE_VIRTUAL(ABBBEquipment::ShutdownRuntimeData, );

    /** @param Packet 通用次行为请求 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** 由具体装备根创建自己的协议组件 */
    UPROPERTY(VisibleAnywhere, Category = "BBB|装备", meta = (DisplayName = "装备协议组件"))
    TObjectPtr<UBBBEquipmentNetworkComponent> NetworkComponent = nullptr;

    /** @param Packet	装备表现请求 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** @param Packet	权威装备表现请求 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** @param Packet	主行为请求 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** @param Packet\t持有者操作许可 @return 无 仅贡献输入 */

    /** @param Packet BeginAction动画事实 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** @param Packet EndAction动画事实 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** @param Packet BeginContact动画事实 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

    /** @param Packet EndContact动画事实 @return 无 仅贡献输入 */
    virtual void QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBEquipment::QueueInput, );

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
