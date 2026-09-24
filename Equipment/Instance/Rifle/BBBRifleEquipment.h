#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBRifleEquipment.generated.h"

/** 步枪实例的唯一运行时根 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBRifleEquipment : public ABBBEquipment
{
    GENERATED_BODY()

public:
    ABBBRifleEquipment();

    /** 步枪唯一聚合黑板 */
    FBBBRifleRuntimeData RuntimeData;

    /** 提交装备表现 @return 无 */
    virtual void SubmitEquipInput() override;

    /** 提交本机开火请求 @return 无 */
    virtual void SubmitPrimaryInput() override;

    /** 提交本机换弹请求 @return 无 */
    virtual void SubmitReloadInput() override;

    /** 卸下时收束自身状态 @return 无 */
    virtual void OnUnequipped() override;

    /** @param Data	收到的完整步枪状态 @return 是否接受 */
    virtual bool SubmitNetworkPayload(const TArray<uint8> &Data) override;

    /** 提交本机弹匣卸下通知 @return 无 */
    void SubmitDetachMagazineInput();

    /** 提交本机弹匣装入通知 @return 无 */
    void SubmitLoadMagazineInput();

    /** 提交本机换弹结束通知 @return 无 */
    void SubmitInterruptReloadInput();

protected:
    /** @param MuzzleTransform	本次开火枪口世界变换 @return 无 */
    UFUNCTION(BlueprintNativeEvent, Category = "BBB|Rifle")
    void EmitShot(const FTransform &MuzzleTransform);

    virtual void EmitShot_Implementation(const FTransform &MuzzleTransform);

private:
    friend class FBBBRifleActionProcessor;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool InitializeRuntimeData() override;
};
