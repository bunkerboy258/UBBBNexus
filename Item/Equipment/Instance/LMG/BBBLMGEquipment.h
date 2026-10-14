#pragma once

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/RuntimeData/BBBLMGRuntimeData.h"
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
struct FBBBLMGActionPermissionLocalControlPacket;
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
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread(), TEXT("轻机枪输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
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
    friend class FBBBLMGActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket<> Packet) override;
    virtual bool QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) override;
    bool QueueInput(FBBBLMGEquipLocalControlPacket Packet);
    bool QueueInput(FBBBLMGEquipAuthorityFactPacket Packet);
    bool QueueInput(FBBBLMGPrimaryLocalControlPacket Packet);
    bool QueueInput(FBBBLMGSecondaryLocalControlPacket Packet);
    bool QueueInput(FBBBLMGBlockFireLocalControlPacket Packet);
    bool QueueInput(FBBBLMGAllowFireLocalControlPacket Packet);
    bool QueueInput(FBBBLMGLoadAmmoLocalControlPacket Packet);
    bool QueueInput(FBBBLMGInterruptReloadLocalControlPacket Packet);
    bool QueueInput(FBBBLMGActionPermissionLocalControlPacket Packet);
    bool QueueInput(FBBBLMGFireRemoteMessagePacket Packet);
    bool QueueInput(FBBBLMGReloadStartRemoteMessagePacket Packet);
    bool QueueInput(FBBBLMGReloadEndRemoteMessagePacket Packet);
    bool QueueInput(FBBBLMGFireAuthorityFactPacket Packet);
    bool QueueInput(FBBBLMGReloadStartAuthorityFactPacket Packet);
    bool QueueInput(FBBBLMGReloadEndAuthorityFactPacket Packet);
};
