#pragma once

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/RuntimeData/BBBPistolRuntimeData.h"
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
struct FBBBPistolActionPermissionLocalControlPacket;
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
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread(), TEXT("手枪输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
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
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual bool QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet) override;
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual bool QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet) override;
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual bool QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet) override;
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual bool QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet) override;
    using ABBBEquipment::QueueInput;
    virtual bool QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet) override;
    friend class FBBBPistolActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) override;
    bool QueueInput(FBBBPistolEquipLocalControlPacket Packet);
    bool QueueInput(FBBBPistolEquipAuthorityFactPacket Packet);
    bool QueueInput(FBBBPistolPrimaryLocalControlPacket Packet);
    bool QueueInput(FBBBPistolSecondaryLocalControlPacket Packet);
    bool QueueInput(FBBBPistolBlockFireLocalControlPacket Packet);
    bool QueueInput(FBBBPistolAllowFireLocalControlPacket Packet);
    bool QueueInput(FBBBPistolLoadAmmoLocalControlPacket Packet);
    bool QueueInput(FBBBPistolInterruptReloadLocalControlPacket Packet);
    bool QueueInput(FBBBPistolActionPermissionLocalControlPacket Packet);
    bool QueueInput(FBBBPistolFireRemoteMessagePacket Packet);
    bool QueueInput(FBBBPistolReloadStartRemoteMessagePacket Packet);
    bool QueueInput(FBBBPistolReloadEndRemoteMessagePacket Packet);
    bool QueueInput(FBBBPistolFireAuthorityFactPacket Packet);
    bool QueueInput(FBBBPistolReloadStartAuthorityFactPacket Packet);
    bool QueueInput(FBBBPistolReloadEndAuthorityFactPacket Packet);
};
