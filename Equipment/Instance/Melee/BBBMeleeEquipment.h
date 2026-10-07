#pragma once
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/RuntimeData/BBBMeleeRuntimeData.h"
#include "BBBMeleeEquipment.generated.h"
struct FBBBMeleeEquipLocalControlPacket;
struct FBBBMeleeEquipAuthorityFactPacket;
struct FBBBMeleeUnequipLocalControlPacket;
struct FBBBMeleeUnequipAuthorityFactPacket;
struct FBBBMeleeAttackLocalControlPacket;
struct FBBBMeleeActionPermissionLocalControlPacket;
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
    /** @return 无 构造近战装备并建立更新时序 */
    ABBBMeleeEquipment();
    /** @param Packet	输入数据 @return 是否接受 */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("近战输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
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
    using ABBBEquipment::QueueInput;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
    virtual void ShutdownRuntimeData() override;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentUnequipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentUnequipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentBeginActionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEndActionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentBeginContactLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEndContactLocalControlPacket Packet) override;
    bool QueueInput(FBBBMeleeEquipLocalControlPacket Packet);
    bool QueueInput(FBBBMeleeEquipAuthorityFactPacket Packet);
    bool QueueInput(FBBBMeleeUnequipLocalControlPacket Packet);
    bool QueueInput(FBBBMeleeUnequipAuthorityFactPacket Packet);
    bool QueueInput(FBBBMeleeAttackLocalControlPacket Packet);
    bool QueueInput(FBBBMeleeActionPermissionLocalControlPacket Packet);
    bool QueueInput(FBBBMeleeBeginActionLocalControlPacket Packet);
    bool QueueInput(FBBBMeleeBeginContactLocalControlPacket Packet);
    bool QueueInput(FBBBMeleeEndContactLocalControlPacket Packet);
    bool QueueInput(FBBBMeleeEndActionLocalControlPacket Packet);
    bool QueueInput(FBBBMeleeAttackStartRemoteMessagePacket Packet);
    bool QueueInput(FBBBMeleeAttackEndRemoteMessagePacket Packet);
    bool QueueInput(FBBBMeleeAttackStartAuthorityFactPacket Packet);
    bool QueueInput(FBBBMeleeAttackEndAuthorityFactPacket Packet);
};
