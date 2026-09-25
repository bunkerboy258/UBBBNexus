#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBRifleEquipment.generated.h"

struct FBBBRifleDetachMagazinePacket;
struct FBBBRifleLoadMagazinePacket;
struct FBBBRifleInterruptReloadPacket;

/** 步枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBRifleEquipment : public ABBBEquipment
{
    GENERATED_BODY()

public:
    ABBBRifleEquipment();

    /** 步枪唯一聚合黑板 */
    FBBBRifleRuntimeData RuntimeData;

    using ABBBEquipment::SubmitInput;

    /** @param Packet	弹匣卸下通知 @return 是否接受 */
    bool SubmitInput(FBBBRifleDetachMagazinePacket &&Packet);

    /** @param Packet	弹匣装入通知 @return 是否接受 */
    bool SubmitInput(FBBBRifleLoadMagazinePacket &&Packet);

    /** @param Packet	换弹结束通知 @return 是否接受 */
    bool SubmitInput(FBBBRifleInterruptReloadPacket &&Packet);

    /** 卸下时收束自身状态 @return 无 */
    virtual void OnUnequipped() override;

protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|Rifle")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    friend class FBBBRifleActionProcessor;

    virtual bool QueueInput(FBBBEquipmentEquipPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentPrimaryPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentReloadPacket Packet) override;
    virtual bool QueueInput(FBBBEquipmentNetworkPayload Payload) override;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
};
