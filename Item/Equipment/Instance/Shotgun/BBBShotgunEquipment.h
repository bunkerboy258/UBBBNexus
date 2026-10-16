#pragma once

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/RuntimeData/BBBShotgunRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/RemoteMessage/Equipment/FBBBShotgunVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/AuthorityFact/Equipment/FBBBShotgunVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunEndContactLocalControlPacket.h"
#include "BBBShotgunEquipment.generated.h"


struct FBBBShotgunEquipLocalControlPacket;
struct FBBBShotgunEquipAuthorityFactPacket;
struct FBBBShotgunPrimaryLocalControlPacket;
struct FBBBShotgunSecondaryLocalControlPacket;
struct FBBBShotgunBlockFireLocalControlPacket;
struct FBBBShotgunAllowFireLocalControlPacket;
struct FBBBShotgunLoadAmmoLocalControlPacket;
struct FBBBShotgunInterruptReloadLocalControlPacket;
struct FBBBShotgunReloadCycleEndLocalControlPacket;
struct FBBBShotgunFireRemoteMessagePacket;
struct FBBBShotgunReloadStartRemoteMessagePacket;
struct FBBBShotgunReloadEndRemoteMessagePacket;
struct FBBBShotgunFireAuthorityFactPacket;
struct FBBBShotgunReloadStartAuthorityFactPacket;
struct FBBBShotgunReloadEndAuthorityFactPacket;

/** 霰弹枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBShotgunEquipment final : public ABBBEquipment
{
    GENERATED_BODY()

public:
    virtual bool IsMeshHidden() const override;
    /** @return 换弹和动作阻塞是否允许快捷消耗品打断 */
    virtual bool CanQuickUse() const override;
    ABBBShotgunEquipment();

    /**
     * 按霰弹枪开火配置读取同一个枪口 不存在时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const override;

    /** @param Loaded	当前弹量 @param Capacity	弹匣容量 @param bContinuous	连续弹药弧 @return 是否具有弹匣显示 */
    virtual bool ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const override;

    /** 霰弹枪唯一聚合黑板 */
    FBBBShotgunRuntimeData RuntimeData;

    /**
     * 提交霰弹枪输入
     * @param Packet	待提交的输入包
     * @return 无 仅贡献输入
     */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 当前弹匣内弹量 */
    UFUNCTION(BlueprintPure, Category = "BBB|霰弹枪")
    int32 GetLoadedAmmo() const
    {
        return RuntimeData.Action.ReadShotgunActionState().LoadedAmmo;
    }

    /** @return 当前弹匣容量 */
    UFUNCTION(BlueprintPure, Category = "BBB|霰弹枪")
    int32 GetAmmoCapacity() const
    {
        return RuntimeData.Action.ReadShotgunActionState().AmmoCapacity;
    }

    /** @return 当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|霰弹枪")
    bool IsReloading() const
    {
        return RuntimeData.Action.ReadShotgunActionState().bIsReloading;
    }


protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|霰弹枪")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    virtual void QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet) override;
    void QueueInput(FBBBShotgunVisibilityAuthorityFactPacket Packet);
    virtual void QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet) override;
    void QueueInput(FBBBShotgunVisibilityRemoteMessagePacket Packet);
    virtual void QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet) override;
    void QueueInput(FBBBShotgunShowLocalControlPacket Packet);
    virtual void QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet) override;
    void QueueInput(FBBBShotgunHideLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBShotgunBeginActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBShotgunBeginContactLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBShotgunEndActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBShotgunEndContactLocalControlPacket Packet);
    using ABBBEquipment::QueueInput;
    virtual void QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet) override;
    friend class FBBBShotgunActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual void QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) override;
    void QueueInput(FBBBShotgunEquipLocalControlPacket Packet);
    void QueueInput(FBBBShotgunEquipAuthorityFactPacket Packet);
    void QueueInput(FBBBShotgunPrimaryLocalControlPacket Packet);
    void QueueInput(FBBBShotgunSecondaryLocalControlPacket Packet);
    void QueueInput(FBBBShotgunBlockFireLocalControlPacket Packet);
    void QueueInput(FBBBShotgunAllowFireLocalControlPacket Packet);
    void QueueInput(FBBBShotgunLoadAmmoLocalControlPacket Packet);
    void QueueInput(FBBBShotgunInterruptReloadLocalControlPacket Packet);
    void QueueInput(FBBBShotgunReloadCycleEndLocalControlPacket Packet);
    void QueueInput(FBBBShotgunFireRemoteMessagePacket Packet);
    void QueueInput(FBBBShotgunReloadStartRemoteMessagePacket Packet);
    void QueueInput(FBBBShotgunReloadEndRemoteMessagePacket Packet);
    void QueueInput(FBBBShotgunFireAuthorityFactPacket Packet);
    void QueueInput(FBBBShotgunReloadStartAuthorityFactPacket Packet);
    void QueueInput(FBBBShotgunReloadEndAuthorityFactPacket Packet);
};
