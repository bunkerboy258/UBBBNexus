#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/RuntimeData/BBBSniperRuntimeData.h"
#include "BBBSniperEquipment.generated.h"


struct FBBBSniperEquipLocalControlPacket;
struct FBBBSniperEquipAuthorityFactPacket;
struct FBBBSniperFireLocalControlPacket;
struct FBBBSniperReloadLocalControlPacket;
struct FBBBSniperBlockFireLocalControlPacket;
struct FBBBSniperAllowFireLocalControlPacket;
struct FBBBSniperLoadMagazineLocalControlPacket;
struct FBBBSniperInterruptReloadLocalControlPacket;
struct FBBBSniperActionPermissionLocalControlPacket;
struct FBBBSniperFireRemoteMessagePacket;
struct FBBBSniperReloadStartRemoteMessagePacket;
struct FBBBSniperReloadEndRemoteMessagePacket;
struct FBBBSniperFireAuthorityFactPacket;
struct FBBBSniperReloadStartAuthorityFactPacket;
struct FBBBSniperReloadEndAuthorityFactPacket;

/** 狙击枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBSniperEquipment final : public ABBBEquipment
{
    GENERATED_BODY()

public:
    ABBBSniperEquipment();

    /**
     * 按狙击枪开火配置读取同一个枪口 不存在时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const override;

    /** @param Loaded	当前弹量 @param Capacity	弹匣容量 @param bContinuous	连续弹药弧 @return 是否具有弹匣显示 */
    virtual bool ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const override;

    /** 狙击枪唯一聚合黑板 */
    FBBBSniperRuntimeData RuntimeData;

    /**
     * 提交狙击枪输入
     * @param Packet	待提交的输入包
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("狙击枪输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 当前弹匣内弹量 */
    UFUNCTION(BlueprintPure, Category = "BBB|狙击枪")
    int32 GetLoadedAmmo() const
    {
        return RuntimeData.Action.ReadSniperActionState().LoadedAmmo;
    }

    /** @return 当前弹匣容量 */
    UFUNCTION(BlueprintPure, Category = "BBB|狙击枪")
    int32 GetAmmoCapacity() const
    {
        return RuntimeData.Action.ReadSniperActionState().AmmoCapacity;
    }

    /** @return 当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|狙击枪")
    bool IsReloading() const
    {
        return RuntimeData.Action.ReadSniperActionState().bIsReloading;
    }


protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|狙击枪")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    using ABBBEquipment::QueueInput;
    virtual bool QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet) override;
    friend class FBBBSniperActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet) override;
    bool QueueInput(FBBBSniperEquipLocalControlPacket Packet);
    bool QueueInput(FBBBSniperEquipAuthorityFactPacket Packet);
    bool QueueInput(FBBBSniperFireLocalControlPacket Packet);
    bool QueueInput(FBBBSniperReloadLocalControlPacket Packet);
    bool QueueInput(FBBBSniperBlockFireLocalControlPacket Packet);
    bool QueueInput(FBBBSniperAllowFireLocalControlPacket Packet);
    bool QueueInput(FBBBSniperLoadMagazineLocalControlPacket Packet);
    bool QueueInput(FBBBSniperInterruptReloadLocalControlPacket Packet);
    bool QueueInput(FBBBSniperActionPermissionLocalControlPacket Packet);
    bool QueueInput(FBBBSniperFireRemoteMessagePacket Packet);
    bool QueueInput(FBBBSniperReloadStartRemoteMessagePacket Packet);
    bool QueueInput(FBBBSniperReloadEndRemoteMessagePacket Packet);
    bool QueueInput(FBBBSniperFireAuthorityFactPacket Packet);
    bool QueueInput(FBBBSniperReloadStartAuthorityFactPacket Packet);
    bool QueueInput(FBBBSniperReloadEndAuthorityFactPacket Packet);
};
