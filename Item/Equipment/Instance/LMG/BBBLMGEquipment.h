#pragma once

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/RuntimeData/BBBLMGRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/RemoteMessage/Equipment/FBBBLMGVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/AuthorityFact/Equipment/FBBBLMGVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGEndContactLocalControlPacket.h"
#include "BBBLMGEquipment.generated.h"


struct FBBBLMGEquipLocalControlPacket;
struct FBBBLMGEquipAuthorityFactPacket;
struct FBBBLMGPrimaryLocalControlPacket;
struct FBBBLMGSecondaryLocalControlPacket;
struct FBBBLMGBlockFireLocalControlPacket;
struct FBBBLMGAllowFireLocalControlPacket;
struct FBBBLMGLoadAmmoLocalControlPacket;
struct FBBBLMGInterruptReloadLocalControlPacket;
struct FBBBLMGFireRemoteMessagePacket;
struct FBBBLMGReloadStartRemoteMessagePacket;
struct FBBBLMGReloadEndRemoteMessagePacket;
struct FBBBLMGFireAuthorityFactPacket;
struct FBBBLMGReloadStartAuthorityFactPacket;
struct FBBBLMGReloadEndAuthorityFactPacket;

/** 轻机枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBLMGEquipment final : public ABBBEquipment
{
    GENERATED_BODY()

public:
    virtual bool IsMeshHidden() const override;
    /** @return 换弹和动作阻塞是否允许快捷消耗品打断 */
    virtual bool CanQuickUse() const override;
    ABBBLMGEquipment();

    /**
     * 按轻机枪开火配置读取同一个枪口 不存在时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const override;

    /** @param Loaded	当前弹量 @param Capacity	弹匣容量 @param bContinuous	连续弹药弧 @return 是否具有弹匣显示 */
    virtual bool ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const override;

    /** 轻机枪唯一聚合黑板 */
    FBBBLMGRuntimeData RuntimeData;

    /**
     * 提交轻机枪输入
     * @param Packet	待提交的输入包
     * @return 无 仅贡献输入
     */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 当前弹匣内弹量 */
    UFUNCTION(BlueprintPure, Category = "BBB|轻机枪")
    int32 GetLoadedAmmo() const
    {
        return RuntimeData.Action.ReadLMGActionState().LoadedAmmo;
    }

    /** @return 当前弹匣容量 */
    UFUNCTION(BlueprintPure, Category = "BBB|轻机枪")
    int32 GetAmmoCapacity() const
    {
        return RuntimeData.Action.ReadLMGActionState().AmmoCapacity;
    }

    /** @return 当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|轻机枪")
    bool IsReloading() const
    {
        return RuntimeData.Action.ReadLMGActionState().bIsReloading;
    }


protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|轻机枪")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    virtual void QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet) override;
    void QueueInput(FBBBLMGVisibilityAuthorityFactPacket Packet);
    virtual void QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet) override;
    void QueueInput(FBBBLMGVisibilityRemoteMessagePacket Packet);
    virtual void QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet) override;
    void QueueInput(FBBBLMGShowLocalControlPacket Packet);
    virtual void QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet) override;
    void QueueInput(FBBBLMGHideLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBLMGBeginActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBLMGBeginContactLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBLMGEndActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBLMGEndContactLocalControlPacket Packet);
    using ABBBEquipment::QueueInput;
    virtual void QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet) override;
    friend class FBBBLMGActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual void QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) override;
    void QueueInput(FBBBLMGEquipLocalControlPacket Packet);
    void QueueInput(FBBBLMGEquipAuthorityFactPacket Packet);
    void QueueInput(FBBBLMGPrimaryLocalControlPacket Packet);
    void QueueInput(FBBBLMGSecondaryLocalControlPacket Packet);
    void QueueInput(FBBBLMGBlockFireLocalControlPacket Packet);
    void QueueInput(FBBBLMGAllowFireLocalControlPacket Packet);
    void QueueInput(FBBBLMGLoadAmmoLocalControlPacket Packet);
    void QueueInput(FBBBLMGInterruptReloadLocalControlPacket Packet);
    void QueueInput(FBBBLMGFireRemoteMessagePacket Packet);
    void QueueInput(FBBBLMGReloadStartRemoteMessagePacket Packet);
    void QueueInput(FBBBLMGReloadEndRemoteMessagePacket Packet);
    void QueueInput(FBBBLMGFireAuthorityFactPacket Packet);
    void QueueInput(FBBBLMGReloadStartAuthorityFactPacket Packet);
    void QueueInput(FBBBLMGReloadEndAuthorityFactPacket Packet);
};
