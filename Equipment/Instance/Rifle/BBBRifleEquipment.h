#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBRifleEquipment.generated.h"


struct FBBBRifleEquipLocalControlPacket;
struct FBBBRifleUnequipLocalControlPacket;
struct FBBBRifleUnequipAuthorityFactPacket;
struct FBBBRifleEquipAuthorityFactPacket;
struct FBBBRifleFireLocalControlPacket;
struct FBBBRifleReloadLocalControlPacket;
struct FBBBRifleBlockFireLocalControlPacket;
struct FBBBRifleAllowFireLocalControlPacket;
struct FBBBRifleLoadMagazineLocalControlPacket;
struct FBBBRifleInterruptReloadLocalControlPacket;
struct FBBBRifleActionPermissionLocalControlPacket;
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
    ABBBRifleEquipment();

    /**
     * 按步枪开火配置读取同一个枪口 不存在时正常返回失败
     * @param OutTransform	成功时写入枪口世界变换 失败时清为单位变换
     * @return 是否存在有效枪口
     */
    virtual bool TryGetMuzzleTransform(FTransform &OutTransform) const override;

    /** 步枪唯一聚合黑板 */
    FBBBRifleRuntimeData RuntimeData;

    /**
     * 提交步枪输入
     * @param Packet	待提交的输入包
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("步枪输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
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
    using ABBBEquipment::QueueInput;
    friend class FBBBRifleActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentUnequipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentUnequipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet) override;
    bool QueueInput(FBBBRifleEquipLocalControlPacket Packet);
    bool QueueInput(FBBBRifleUnequipLocalControlPacket Packet);
    bool QueueInput(FBBBRifleUnequipAuthorityFactPacket Packet);
    bool QueueInput(FBBBRifleEquipAuthorityFactPacket Packet);
    bool QueueInput(FBBBRifleFireLocalControlPacket Packet);
    bool QueueInput(FBBBRifleReloadLocalControlPacket Packet);
    bool QueueInput(FBBBRifleBlockFireLocalControlPacket Packet);
    bool QueueInput(FBBBRifleAllowFireLocalControlPacket Packet);
    bool QueueInput(FBBBRifleLoadMagazineLocalControlPacket Packet);
    bool QueueInput(FBBBRifleInterruptReloadLocalControlPacket Packet);
    bool QueueInput(FBBBRifleActionPermissionLocalControlPacket Packet);
    bool QueueInput(FBBBRifleFireRemoteMessagePacket Packet);
    bool QueueInput(FBBBRifleReloadStartRemoteMessagePacket Packet);
    bool QueueInput(FBBBRifleReloadEndRemoteMessagePacket Packet);
    bool QueueInput(FBBBRifleFireAuthorityFactPacket Packet);
    bool QueueInput(FBBBRifleReloadStartAuthorityFactPacket Packet);
    bool QueueInput(FBBBRifleReloadEndAuthorityFactPacket Packet);
};
