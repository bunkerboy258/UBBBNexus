#pragma once

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/RemoteMessage/Equipment/FBBBRifleVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/AuthorityFact/Equipment/FBBBRifleVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleEndContactLocalControlPacket.h"
#include "BBBRifleEquipment.generated.h"


struct FBBBRifleEquipLocalControlPacket;
struct FBBBRifleEquipAuthorityFactPacket;
struct FBBBRiflePrimaryLocalControlPacket;
struct FBBBRifleSecondaryLocalControlPacket;
struct FBBBRifleBlockFireLocalControlPacket;
struct FBBBRifleAllowFireLocalControlPacket;
struct FBBBRifleLoadAmmoLocalControlPacket;
struct FBBBRifleInterruptReloadLocalControlPacket;
struct FBBBRifleFireRemoteMessagePacket;
struct FBBBRifleReloadStartRemoteMessagePacket;
struct FBBBRifleReloadEndRemoteMessagePacket;
struct FBBBRifleFireAuthorityFactPacket;
struct FBBBRifleReloadStartAuthorityFactPacket;
struct FBBBRifleReloadEndAuthorityFactPacket;

/** 步枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBRifleEquipment final : public ABBBEquipment
{
    GENERATED_BODY()

public:
    virtual bool IsMeshHidden() const override;
    /** @return 换弹和动作阻塞是否允许快捷消耗品打断 */
    virtual bool CanQuickUse() const override;
    ABBBRifleEquipment();

    /**
     * 按步枪开火配置读取同一个枪口 不存在时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const override;

    /** @param Loaded	当前弹量 @param Capacity	弹匣容量 @param bContinuous	连续弹药弧 @return 是否具有弹匣显示 */
    virtual bool ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const override;

    /** 步枪唯一聚合黑板 */
    FBBBRifleRuntimeData RuntimeData;

    /**
     * 提交步枪输入
     * @param Packet	待提交的输入包
     * @return 无 仅贡献输入
     */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 当前弹匣内弹量 */
    UFUNCTION(BlueprintPure, Category = "BBB|步枪")
    int32 GetLoadedAmmo() const
    {
        return RuntimeData.Action.ReadRifleActionState().LoadedAmmo;
    }

    /** @return 当前弹匣容量 */
    UFUNCTION(BlueprintPure, Category = "BBB|步枪")
    int32 GetAmmoCapacity() const
    {
        return RuntimeData.Action.ReadRifleActionState().AmmoCapacity;
    }

    /** @return 当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|步枪")
    bool IsReloading() const
    {
        return RuntimeData.Action.ReadRifleActionState().bIsReloading;
    }


protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|步枪")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    virtual void QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet) override;
    void QueueInput(FBBBRifleVisibilityAuthorityFactPacket Packet);
    virtual void QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet) override;
    void QueueInput(FBBBRifleVisibilityRemoteMessagePacket Packet);
    virtual void QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRifleShowLocalControlPacket Packet);
    virtual void QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRifleHideLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRifleBeginActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRifleBeginContactLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRifleEndActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRifleEndContactLocalControlPacket Packet);
    using ABBBEquipment::QueueInput;
    virtual void QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet) override;
    friend class FBBBRifleActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual void QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) override;
    void QueueInput(FBBBRifleEquipLocalControlPacket Packet);
    void QueueInput(FBBBRifleEquipAuthorityFactPacket Packet);
    void QueueInput(FBBBRiflePrimaryLocalControlPacket Packet);
    void QueueInput(FBBBRifleSecondaryLocalControlPacket Packet);
    void QueueInput(FBBBRifleBlockFireLocalControlPacket Packet);
    void QueueInput(FBBBRifleAllowFireLocalControlPacket Packet);
    void QueueInput(FBBBRifleLoadAmmoLocalControlPacket Packet);
    void QueueInput(FBBBRifleInterruptReloadLocalControlPacket Packet);
    void QueueInput(FBBBRifleFireRemoteMessagePacket Packet);
    void QueueInput(FBBBRifleReloadStartRemoteMessagePacket Packet);
    void QueueInput(FBBBRifleReloadEndRemoteMessagePacket Packet);
    void QueueInput(FBBBRifleFireAuthorityFactPacket Packet);
    void QueueInput(FBBBRifleReloadStartAuthorityFactPacket Packet);
    void QueueInput(FBBBRifleReloadEndAuthorityFactPacket Packet);
};
