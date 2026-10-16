#pragma once

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/RuntimeData/BBBMinigunRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/RemoteMessage/Equipment/FBBBMinigunVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/AuthorityFact/Equipment/FBBBMinigunVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunEndContactLocalControlPacket.h"
#include "BBBMinigunEquipment.generated.h"


struct FBBBMinigunEquipLocalControlPacket;
struct FBBBMinigunEquipAuthorityFactPacket;
struct FBBBMinigunPrimaryLocalControlPacket;
struct FBBBMinigunSecondaryLocalControlPacket;
struct FBBBMinigunBlockFireLocalControlPacket;
struct FBBBMinigunAllowFireLocalControlPacket;
struct FBBBMinigunLoadAmmoLocalControlPacket;
struct FBBBMinigunInterruptReloadLocalControlPacket;
struct FBBBMinigunFireRemoteMessagePacket;
struct FBBBMinigunSpinRemoteMessagePacket;
struct FBBBMinigunSpinAuthorityFactPacket;
struct FBBBMinigunReloadStartRemoteMessagePacket;
struct FBBBMinigunReloadEndRemoteMessagePacket;
struct FBBBMinigunFireAuthorityFactPacket;
struct FBBBMinigunReloadStartAuthorityFactPacket;
struct FBBBMinigunReloadEndAuthorityFactPacket;

/** 转管机枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBMinigunEquipment final : public ABBBEquipment
{
    GENERATED_BODY()

public:
    virtual bool IsMeshHidden() const override;
    /** @return 换弹和动作阻塞是否允许快捷消耗品打断 */
    virtual bool CanQuickUse() const override;
    ABBBMinigunEquipment();

    /**
     * 按转管机枪开火配置读取同一个枪口 不存在时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const override;

    /** @param Loaded	当前弹量 @param Capacity	弹匣容量 @param bContinuous	连续弹药弧 @return 是否具有弹匣显示 */
    virtual bool ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const override;

    /** 转管机枪唯一聚合黑板 */
    FBBBMinigunRuntimeData RuntimeData;

    /**
     * 提交转管机枪输入
     * @param Packet	待提交的输入包
     * @return 无 仅贡献输入
     */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 当前弹匣内弹量 */
    UFUNCTION(BlueprintPure, Category = "BBB|转管机枪")
    int32 GetLoadedAmmo() const
    {
        return RuntimeData.Action.ReadMinigunActionState().LoadedAmmo;
    }

    /** @return 当前弹匣容量 */
    UFUNCTION(BlueprintPure, Category = "BBB|转管机枪")
    int32 GetAmmoCapacity() const
    {
        return RuntimeData.Action.ReadMinigunActionState().AmmoCapacity;
    }

    /** @return 当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|转管机枪")
    bool IsReloading() const
    {
        return RuntimeData.Action.ReadMinigunActionState().bIsReloading;
    }


protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|转管机枪")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    virtual void QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet) override;
    void QueueInput(FBBBMinigunVisibilityAuthorityFactPacket Packet);
    virtual void QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet) override;
    void QueueInput(FBBBMinigunVisibilityRemoteMessagePacket Packet);
    virtual void QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMinigunShowLocalControlPacket Packet);
    virtual void QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMinigunHideLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMinigunBeginActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMinigunBeginContactLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMinigunEndActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMinigunEndContactLocalControlPacket Packet);
    using ABBBEquipment::QueueInput;
    virtual void QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet) override;
    friend class FBBBMinigunActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual void QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMinigunEquipLocalControlPacket Packet);
    void QueueInput(FBBBMinigunEquipAuthorityFactPacket Packet);
    void QueueInput(FBBBMinigunPrimaryLocalControlPacket Packet);
    void QueueInput(FBBBMinigunSecondaryLocalControlPacket Packet);
    void QueueInput(FBBBMinigunBlockFireLocalControlPacket Packet);
    void QueueInput(FBBBMinigunAllowFireLocalControlPacket Packet);
    void QueueInput(FBBBMinigunLoadAmmoLocalControlPacket Packet);
    void QueueInput(FBBBMinigunInterruptReloadLocalControlPacket Packet);
    void QueueInput(FBBBMinigunFireRemoteMessagePacket Packet);
    void QueueInput(FBBBMinigunSpinRemoteMessagePacket Packet);
    void QueueInput(FBBBMinigunSpinAuthorityFactPacket Packet);
    void QueueInput(FBBBMinigunReloadStartRemoteMessagePacket Packet);
    void QueueInput(FBBBMinigunReloadEndRemoteMessagePacket Packet);
    void QueueInput(FBBBMinigunFireAuthorityFactPacket Packet);
    void QueueInput(FBBBMinigunReloadStartAuthorityFactPacket Packet);
    void QueueInput(FBBBMinigunReloadEndAuthorityFactPacket Packet);
};
