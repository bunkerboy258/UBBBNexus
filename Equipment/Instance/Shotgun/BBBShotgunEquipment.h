#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/RuntimeData/BBBShotgunRuntimeData.h"
#include "BBBShotgunEquipment.generated.h"


struct FBBBShotgunEquipLocalControlPacket;
struct FBBBShotgunEquipAuthorityFactPacket;
struct FBBBShotgunFireLocalControlPacket;
struct FBBBShotgunReloadLocalControlPacket;
struct FBBBShotgunBlockFireLocalControlPacket;
struct FBBBShotgunAllowFireLocalControlPacket;
struct FBBBShotgunLoadAmmoLocalControlPacket;
struct FBBBShotgunInterruptReloadLocalControlPacket;
struct FBBBShotgunActionPermissionLocalControlPacket;
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
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("霰弹枪输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
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
    using ABBBEquipment::QueueInput;
    virtual bool QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet) override;
    friend class FBBBShotgunActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet) override;
    bool QueueInput(FBBBShotgunEquipLocalControlPacket Packet);
    bool QueueInput(FBBBShotgunEquipAuthorityFactPacket Packet);
    bool QueueInput(FBBBShotgunFireLocalControlPacket Packet);
    bool QueueInput(FBBBShotgunReloadLocalControlPacket Packet);
    bool QueueInput(FBBBShotgunBlockFireLocalControlPacket Packet);
    bool QueueInput(FBBBShotgunAllowFireLocalControlPacket Packet);
    bool QueueInput(FBBBShotgunLoadAmmoLocalControlPacket Packet);
    bool QueueInput(FBBBShotgunInterruptReloadLocalControlPacket Packet);
    bool QueueInput(FBBBShotgunActionPermissionLocalControlPacket Packet);
    bool QueueInput(FBBBShotgunFireRemoteMessagePacket Packet);
    bool QueueInput(FBBBShotgunReloadStartRemoteMessagePacket Packet);
    bool QueueInput(FBBBShotgunReloadEndRemoteMessagePacket Packet);
    bool QueueInput(FBBBShotgunFireAuthorityFactPacket Packet);
    bool QueueInput(FBBBShotgunReloadStartAuthorityFactPacket Packet);
    bool QueueInput(FBBBShotgunReloadEndAuthorityFactPacket Packet);
};
