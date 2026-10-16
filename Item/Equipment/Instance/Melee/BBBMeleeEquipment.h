#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/RuntimeData/BBBMeleeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Fire/FBBBMeleeAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Fire/FBBBMeleeBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Reload/FBBBMeleeInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Reload/FBBBMeleeLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/RemoteMessage/Equipment/FBBBMeleeVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/AuthorityFact/Equipment/FBBBMeleeVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Fire/FBBBMeleeAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Fire/FBBBMeleeBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Reload/FBBBMeleeInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Reload/FBBBMeleeLoadAmmoLocalControlPacket.h"
#include "BBBMeleeEquipment.generated.h"
struct FBBBMeleeEquipLocalControlPacket;
struct FBBBMeleeEquipAuthorityFactPacket;
struct FBBBMeleePrimaryLocalControlPacket;
struct FBBBMeleeBeginActionLocalControlPacket;
struct FBBBMeleeBeginContactLocalControlPacket;
struct FBBBMeleeEndContactLocalControlPacket;
struct FBBBMeleeEndActionLocalControlPacket;
struct FBBBMeleeAttackStartRemoteMessagePacket;
struct FBBBMeleeAttackEndRemoteMessagePacket;
struct FBBBMeleeAttackStartAuthorityFactPacket;
struct FBBBMeleeAttackEndAuthorityFactPacket;

/** 近战装备唯一运行时根 */
UCLASS(Blueprintable, meta = (DisplayName = "近战装备"))
class ABBB_EVAC_API ABBBMeleeEquipment final : public ABBBEquipment
{
    GENERATED_BODY()
public:
    virtual bool IsMeshHidden() const override;
    /** @return 当前是否已经结束攻击动作 */
    virtual bool CanSwitch() const override;
    /** @return 无 构造近战装备并建立更新时序 */
    ABBBMeleeEquipment();
    /** @param Packet	输入数据 @return 无 仅贡献输入 */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }

    /** 近战唯一聚合黑板 */
    FBBBMeleeRuntimeData RuntimeData;
    /** @return 当前攻击序号 */
    UFUNCTION(BlueprintPure, Category = "BBB|近战", meta = (DisplayName = "攻击序号"))
    int32 GetAttackSequence() const;
    /** @return 当前伤害窗口是否开启 */
    UFUNCTION(BlueprintPure, Category = "BBB|近战", meta = (DisplayName = "伤害窗口开启"))
    bool IsContactOpen() const;
    /** @return 已成立命中数 */
    UFUNCTION(BlueprintPure, Category = "BBB|近战", meta = (DisplayName = "累计命中数"))
    int32 GetHitCount() const;
private:
    virtual void QueueInput(FBBBEquipmentVisibilityAuthorityFactPacket<> Packet) override;
    void QueueInput(FBBBMeleeVisibilityAuthorityFactPacket Packet);
    virtual void QueueInput(FBBBEquipmentVisibilityRemoteMessagePacket<> Packet) override;
    void QueueInput(FBBBMeleeVisibilityRemoteMessagePacket Packet);
    virtual void QueueInput(FBBBEquipmentShowLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMeleeShowLocalControlPacket Packet);
    virtual void QueueInput(FBBBEquipmentHideLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMeleeHideLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentSecondaryLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMeleeSecondaryLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentAllowFireLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMeleeAllowFireLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentBlockFireLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMeleeBlockFireLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentInterruptReloadLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMeleeInterruptReloadLocalControlPacket Packet);
    /** @param Packet 不支持的公共操作 @return 拒绝 */
    virtual void QueueInput(FBBBEquipmentLoadAmmoLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMeleeLoadAmmoLocalControlPacket Packet);
    using ABBBEquipment::QueueInput;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual void QueueInput(FBBBEquipmentEquipLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentEquipAuthorityFactPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentPrimaryLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentBeginActionLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentEndActionLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentBeginContactLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBEquipmentEndContactLocalControlPacket<> Packet) override;
    void QueueInput(FBBBMeleeEquipLocalControlPacket Packet);
    void QueueInput(FBBBMeleeEquipAuthorityFactPacket Packet);
    void QueueInput(FBBBMeleePrimaryLocalControlPacket Packet);
    void QueueInput(FBBBMeleeBeginActionLocalControlPacket Packet);
    void QueueInput(FBBBMeleeBeginContactLocalControlPacket Packet);
    void QueueInput(FBBBMeleeEndContactLocalControlPacket Packet);
    void QueueInput(FBBBMeleeEndActionLocalControlPacket Packet);
    void QueueInput(FBBBMeleeAttackStartRemoteMessagePacket Packet);
    void QueueInput(FBBBMeleeAttackEndRemoteMessagePacket Packet);
    void QueueInput(FBBBMeleeAttackStartAuthorityFactPacket Packet);
    void QueueInput(FBBBMeleeAttackEndAuthorityFactPacket Packet);
};
