#pragma once

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/RuntimeData/BBBRevolverRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/RemoteMessage/Equipment/FBBBRevolverVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/AuthorityFact/Equipment/FBBBRevolverVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverEndContactLocalControlPacket.h"
#include "BBBRevolverEquipment.generated.h"


struct FBBBRevolverEquipLocalControlPacket;
struct FBBBRevolverEquipAuthorityFactPacket;
struct FBBBRevolverPrimaryLocalControlPacket;
struct FBBBRevolverSecondaryLocalControlPacket;
struct FBBBRevolverBlockFireLocalControlPacket;
struct FBBBRevolverAllowFireLocalControlPacket;
struct FBBBRevolverLoadAmmoLocalControlPacket;
struct FBBBRevolverInterruptReloadLocalControlPacket;
struct FBBBRevolverFireRemoteMessagePacket;
struct FBBBRevolverReloadStartRemoteMessagePacket;
struct FBBBRevolverReloadEndRemoteMessagePacket;
struct FBBBRevolverFireAuthorityFactPacket;
struct FBBBRevolverReloadStartAuthorityFactPacket;
struct FBBBRevolverReloadEndAuthorityFactPacket;

/** 左轮实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBRevolverEquipment final : public ABBBEquipment
{
    GENERATED_BODY()

public:
    virtual bool IsMeshHidden() const override;
    /** @return 换弹和动作阻塞是否允许快捷消耗品打断 */
    virtual bool CanQuickUse() const override;
    ABBBRevolverEquipment();

    /**
     * 按左轮开火配置读取同一个枪口 不存在时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const override;

    /** @param Loaded	当前弹量 @param Capacity	弹匣容量 @param bContinuous	连续弹药弧 @return 是否具有弹匣显示 */
    virtual bool ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const override;

    /** 左轮唯一聚合黑板 */
    FBBBRevolverRuntimeData RuntimeData;

    /**
     * 提交左轮输入
     * @param Packet	待提交的输入包
     * @return 无 仅贡献输入
     */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 当前弹匣内弹量 */
    UFUNCTION(BlueprintPure, Category = "BBB|左轮")
    int32 GetLoadedAmmo() const
    {
        return RuntimeData.Action.ReadRevolverActionState().LoadedAmmo;
    }

    /** @return 当前弹匣容量 */
    UFUNCTION(BlueprintPure, Category = "BBB|左轮")
    int32 GetAmmoCapacity() const
    {
        return RuntimeData.Action.ReadRevolverActionState().AmmoCapacity;
    }

    /** @return 当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|左轮")
    bool IsReloading() const
    {
        return RuntimeData.Action.ReadRevolverActionState().bIsReloading;
    }


protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|左轮")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    virtual void QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet) override;
    void QueueInput(FBBBRevolverVisibilityAuthorityFactPacket Packet);
    virtual void QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet) override;
    void QueueInput(FBBBRevolverVisibilityRemoteMessagePacket Packet);
    virtual void QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRevolverShowLocalControlPacket Packet);
    virtual void QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRevolverHideLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRevolverBeginActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRevolverBeginContactLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRevolverEndActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRevolverEndContactLocalControlPacket Packet);
    using ABBBEquipment::QueueInput;
    virtual void QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet) override;
    friend class FBBBRevolverActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual void QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRevolverEquipLocalControlPacket Packet);
    void QueueInput(FBBBRevolverEquipAuthorityFactPacket Packet);
    void QueueInput(FBBBRevolverPrimaryLocalControlPacket Packet);
    void QueueInput(FBBBRevolverSecondaryLocalControlPacket Packet);
    void QueueInput(FBBBRevolverBlockFireLocalControlPacket Packet);
    void QueueInput(FBBBRevolverAllowFireLocalControlPacket Packet);
    void QueueInput(FBBBRevolverLoadAmmoLocalControlPacket Packet);
    void QueueInput(FBBBRevolverInterruptReloadLocalControlPacket Packet);
    void QueueInput(FBBBRevolverFireRemoteMessagePacket Packet);
    void QueueInput(FBBBRevolverReloadStartRemoteMessagePacket Packet);
    void QueueInput(FBBBRevolverReloadEndRemoteMessagePacket Packet);
    void QueueInput(FBBBRevolverFireAuthorityFactPacket Packet);
    void QueueInput(FBBBRevolverReloadStartAuthorityFactPacket Packet);
    void QueueInput(FBBBRevolverReloadEndAuthorityFactPacket Packet);
};
