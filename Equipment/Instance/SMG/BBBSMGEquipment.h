#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/RuntimeData/BBBSMGRuntimeData.h"
#include "BBBSMGEquipment.generated.h"


struct FBBBSMGEquipLocalControlPacket;
struct FBBBSMGEquipAuthorityFactPacket;
struct FBBBSMGFireLocalControlPacket;
struct FBBBSMGReloadLocalControlPacket;
struct FBBBSMGBlockFireLocalControlPacket;
struct FBBBSMGAllowFireLocalControlPacket;
struct FBBBSMGLoadMagazineLocalControlPacket;
struct FBBBSMGInterruptReloadLocalControlPacket;
struct FBBBSMGActionPermissionLocalControlPacket;
struct FBBBSMGFireRemoteMessagePacket;
struct FBBBSMGReloadStartRemoteMessagePacket;
struct FBBBSMGReloadEndRemoteMessagePacket;
struct FBBBSMGFireAuthorityFactPacket;
struct FBBBSMGReloadStartAuthorityFactPacket;
struct FBBBSMGReloadEndAuthorityFactPacket;

/** 冲锋枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBSMGEquipment final : public ABBBEquipment
{
    GENERATED_BODY()

public:
    ABBBSMGEquipment();

    /**
     * 按冲锋枪开火配置读取同一个枪口 不存在时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const override;

    /** @param Loaded	当前弹量 @param Capacity	弹匣容量 @param bContinuous	连续弹药弧 @return 是否具有弹匣显示 */
    virtual bool ReadAmmoDisplay(int32 &Loaded, int32 &Capacity, bool &bContinuous) const override;

    /** 冲锋枪唯一聚合黑板 */
    FBBBSMGRuntimeData RuntimeData;

    /**
     * 提交冲锋枪输入
     * @param Packet	待提交的输入包
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("冲锋枪输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
    }

    /** @return 当前弹匣内弹量 */
    UFUNCTION(BlueprintPure, Category = "BBB|冲锋枪")
    int32 GetLoadedAmmo() const
    {
        return RuntimeData.Action.ReadSMGActionState().LoadedAmmo;
    }

    /** @return 当前弹匣容量 */
    UFUNCTION(BlueprintPure, Category = "BBB|冲锋枪")
    int32 GetAmmoCapacity() const
    {
        return RuntimeData.Action.ReadSMGActionState().AmmoCapacity;
    }

    /** @return 当前是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|冲锋枪")
    bool IsReloading() const
    {
        return RuntimeData.Action.ReadSMGActionState().bIsReloading;
    }


protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|冲锋枪")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    using ABBBEquipment::QueueInput;
    virtual bool QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet) override;
    friend class FBBBSMGActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet) override;
    bool QueueInput(FBBBSMGEquipLocalControlPacket Packet);
    bool QueueInput(FBBBSMGEquipAuthorityFactPacket Packet);
    bool QueueInput(FBBBSMGFireLocalControlPacket Packet);
    bool QueueInput(FBBBSMGReloadLocalControlPacket Packet);
    bool QueueInput(FBBBSMGBlockFireLocalControlPacket Packet);
    bool QueueInput(FBBBSMGAllowFireLocalControlPacket Packet);
    bool QueueInput(FBBBSMGLoadMagazineLocalControlPacket Packet);
    bool QueueInput(FBBBSMGInterruptReloadLocalControlPacket Packet);
    bool QueueInput(FBBBSMGActionPermissionLocalControlPacket Packet);
    bool QueueInput(FBBBSMGFireRemoteMessagePacket Packet);
    bool QueueInput(FBBBSMGReloadStartRemoteMessagePacket Packet);
    bool QueueInput(FBBBSMGReloadEndRemoteMessagePacket Packet);
    bool QueueInput(FBBBSMGFireAuthorityFactPacket Packet);
    bool QueueInput(FBBBSMGReloadStartAuthorityFactPacket Packet);
    bool QueueInput(FBBBSMGReloadEndAuthorityFactPacket Packet);
};
