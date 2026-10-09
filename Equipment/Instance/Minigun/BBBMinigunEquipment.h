#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/RuntimeData/BBBMinigunRuntimeData.h"
#include "BBBMinigunEquipment.generated.h"


struct FBBBMinigunEquipLocalControlPacket;
struct FBBBMinigunEquipAuthorityFactPacket;
struct FBBBMinigunFireLocalControlPacket;
struct FBBBMinigunReloadLocalControlPacket;
struct FBBBMinigunBlockFireLocalControlPacket;
struct FBBBMinigunAllowFireLocalControlPacket;
struct FBBBMinigunLoadMagazineLocalControlPacket;
struct FBBBMinigunInterruptReloadLocalControlPacket;
struct FBBBMinigunActionPermissionLocalControlPacket;
struct FBBBMinigunFireRemoteMessagePacket;
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
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("转管机枪输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
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
    using ABBBEquipment::QueueInput;
    virtual bool QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet) override;
    friend class FBBBMinigunActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet) override;
    bool QueueInput(FBBBMinigunEquipLocalControlPacket Packet);
    bool QueueInput(FBBBMinigunEquipAuthorityFactPacket Packet);
    bool QueueInput(FBBBMinigunFireLocalControlPacket Packet);
    bool QueueInput(FBBBMinigunReloadLocalControlPacket Packet);
    bool QueueInput(FBBBMinigunBlockFireLocalControlPacket Packet);
    bool QueueInput(FBBBMinigunAllowFireLocalControlPacket Packet);
    bool QueueInput(FBBBMinigunLoadMagazineLocalControlPacket Packet);
    bool QueueInput(FBBBMinigunInterruptReloadLocalControlPacket Packet);
    bool QueueInput(FBBBMinigunActionPermissionLocalControlPacket Packet);
    bool QueueInput(FBBBMinigunFireRemoteMessagePacket Packet);
    bool QueueInput(FBBBMinigunReloadStartRemoteMessagePacket Packet);
    bool QueueInput(FBBBMinigunReloadEndRemoteMessagePacket Packet);
    bool QueueInput(FBBBMinigunFireAuthorityFactPacket Packet);
    bool QueueInput(FBBBMinigunReloadStartAuthorityFactPacket Packet);
    bool QueueInput(FBBBMinigunReloadEndAuthorityFactPacket Packet);
};
