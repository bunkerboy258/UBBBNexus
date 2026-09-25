#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBRifleEquipment.generated.h"

struct FBBBRifleDetachMagazineLocalControlPacket;
struct FBBBRifleLoadMagazineLocalControlPacket;
struct FBBBRifleInterruptReloadLocalControlPacket;

/** 步枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBRifleEquipment : public ABBBEquipment
{
    GENERATED_BODY()

public:
    ABBBRifleEquipment();

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

    /** 卸下时收束自身状态 @return 无 */
    virtual void OnUnequipped() override;

protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|Rifle")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    friend class FBBBRifleActionProcessor;

    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentReloadLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentStateAuthorityFactPacket Payload) override;

    /** @param Packet	弹匣卸下通知 @return 是否接受 */
    bool QueueInput(FBBBRifleDetachMagazineLocalControlPacket Packet);

    /** @param Packet	弹匣装入通知 @return 是否接受 */
    bool QueueInput(FBBBRifleLoadMagazineLocalControlPacket Packet);

    /** @param Packet	换弹结束通知 @return 是否接受 */
    bool QueueInput(FBBBRifleInterruptReloadLocalControlPacket Packet);

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
};
