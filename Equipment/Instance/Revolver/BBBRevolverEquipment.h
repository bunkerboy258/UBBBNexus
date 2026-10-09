#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/RuntimeData/BBBRevolverRuntimeData.h"
#include "BBBRevolverEquipment.generated.h"


struct FBBBRevolverEquipLocalControlPacket;
struct FBBBRevolverEquipAuthorityFactPacket;
struct FBBBRevolverFireLocalControlPacket;
struct FBBBRevolverReloadLocalControlPacket;
struct FBBBRevolverBlockFireLocalControlPacket;
struct FBBBRevolverAllowFireLocalControlPacket;
struct FBBBRevolverLoadMagazineLocalControlPacket;
struct FBBBRevolverInterruptReloadLocalControlPacket;
struct FBBBRevolverActionPermissionLocalControlPacket;
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
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("左轮输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
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
    using ABBBEquipment::QueueInput;
    virtual bool QueueInput(FBBBEquipmentBlockFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentAllowFireLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket Packet) override;
    friend class FBBBRevolverActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet) override;
    bool QueueInput(FBBBRevolverEquipLocalControlPacket Packet);
    bool QueueInput(FBBBRevolverEquipAuthorityFactPacket Packet);
    bool QueueInput(FBBBRevolverFireLocalControlPacket Packet);
    bool QueueInput(FBBBRevolverReloadLocalControlPacket Packet);
    bool QueueInput(FBBBRevolverBlockFireLocalControlPacket Packet);
    bool QueueInput(FBBBRevolverAllowFireLocalControlPacket Packet);
    bool QueueInput(FBBBRevolverLoadMagazineLocalControlPacket Packet);
    bool QueueInput(FBBBRevolverInterruptReloadLocalControlPacket Packet);
    bool QueueInput(FBBBRevolverActionPermissionLocalControlPacket Packet);
    bool QueueInput(FBBBRevolverFireRemoteMessagePacket Packet);
    bool QueueInput(FBBBRevolverReloadStartRemoteMessagePacket Packet);
    bool QueueInput(FBBBRevolverReloadEndRemoteMessagePacket Packet);
    bool QueueInput(FBBBRevolverFireAuthorityFactPacket Packet);
    bool QueueInput(FBBBRevolverReloadStartAuthorityFactPacket Packet);
    bool QueueInput(FBBBRevolverReloadEndAuthorityFactPacket Packet);
};
