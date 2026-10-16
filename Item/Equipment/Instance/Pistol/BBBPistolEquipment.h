#pragma once

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/RuntimeData/BBBPistolRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/RemoteMessage/Equipment/FBBBPistolVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/AuthorityFact/Equipment/FBBBPistolVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolEndContactLocalControlPacket.h"
#include "BBBPistolEquipment.generated.h"


struct FBBBPistolEquipLocalControlPacket;
struct FBBBPistolEquipAuthorityFactPacket;
struct FBBBPistolPrimaryLocalControlPacket;
struct FBBBPistolSecondaryLocalControlPacket;
struct FBBBPistolBlockFireLocalControlPacket;
struct FBBBPistolAllowFireLocalControlPacket;
struct FBBBPistolLoadAmmoLocalControlPacket;
struct FBBBPistolInterruptReloadLocalControlPacket;
struct FBBBPistolFireRemoteMessagePacket;
struct FBBBPistolReloadStartRemoteMessagePacket;
struct FBBBPistolReloadEndRemoteMessagePacket;
struct FBBBPistolFireAuthorityFactPacket;
struct FBBBPistolReloadStartAuthorityFactPacket;
struct FBBBPistolReloadEndAuthorityFactPacket;

/** 手枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBPistolEquipment final : public ABBBEquipment
{
    GENERATED_BODY()

public:
    virtual bool IsMeshHidden() const override;
    /** @return 换弹和动作阻塞是否允许快捷消耗品打断 */
    virtual bool CanQuickUse() const override;
    ABBBPistolEquipment();

    /**
     * 按手枪开火配置读取同一个枪口 不存在时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const override;

    /** @param Loaded	当前弹量 @param Capacity	弹匣容量 @param bContinuous	连续弹药弧 @return 是否具有弹匣显示 */
    virtual bool ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const override;

    /** 手枪唯一聚合黑板 */
    FBBBPistolRuntimeData RuntimeData;

    /**
     * 提交手枪输入
     * @param Packet	待提交的输入包
     * @return 无 仅贡献输入
     */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 当前弹匣内弹量 */
    UFUNCTION(BlueprintPure, Category = "BBB|手枪")
    int32 GetLoadedAmmo() const
    {
        return RuntimeData.Action.ReadPistolActionState().LoadedAmmo;
    }

    /** @return 当前弹匣容量 */
    UFUNCTION(BlueprintPure, Category = "BBB|手枪")
    int32 GetAmmoCapacity() const
    {
        return RuntimeData.Action.ReadPistolActionState().AmmoCapacity;
    }

    /** @return 当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|手枪")
    bool IsReloading() const
    {
        return RuntimeData.Action.ReadPistolActionState().bIsReloading;
    }


protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|手枪")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    virtual void QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet) override;
    void QueueInput(FBBBPistolVisibilityAuthorityFactPacket Packet);
    virtual void QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet) override;
    void QueueInput(FBBBPistolVisibilityRemoteMessagePacket Packet);
    virtual void QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet) override;
    void QueueInput(FBBBPistolShowLocalControlPacket Packet);
    virtual void QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet) override;
    void QueueInput(FBBBPistolHideLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBPistolBeginActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBPistolBeginContactLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet) override;
    void QueueInput(FBBBPistolEndActionLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBPistolEndContactLocalControlPacket Packet);
    using ABBBEquipment::QueueInput;
    virtual void QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet) override;
    friend class FBBBPistolActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual void QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) override;
    void QueueInput(FBBBPistolEquipLocalControlPacket Packet);
    void QueueInput(FBBBPistolEquipAuthorityFactPacket Packet);
    void QueueInput(FBBBPistolPrimaryLocalControlPacket Packet);
    void QueueInput(FBBBPistolSecondaryLocalControlPacket Packet);
    void QueueInput(FBBBPistolBlockFireLocalControlPacket Packet);
    void QueueInput(FBBBPistolAllowFireLocalControlPacket Packet);
    void QueueInput(FBBBPistolLoadAmmoLocalControlPacket Packet);
    void QueueInput(FBBBPistolInterruptReloadLocalControlPacket Packet);
    void QueueInput(FBBBPistolFireRemoteMessagePacket Packet);
    void QueueInput(FBBBPistolReloadStartRemoteMessagePacket Packet);
    void QueueInput(FBBBPistolReloadEndRemoteMessagePacket Packet);
    void QueueInput(FBBBPistolFireAuthorityFactPacket Packet);
    void QueueInput(FBBBPistolReloadStartAuthorityFactPacket Packet);
    void QueueInput(FBBBPistolReloadEndAuthorityFactPacket Packet);
};
