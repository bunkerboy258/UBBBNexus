#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/Operation/BBBItemOperation.h"
#include "BBBMagazineOperation.generated.h"

class ABBBEquipmentActor;
class UAnimMontage;
class UBBBMagazineRuntimeData;

/** 弹匣与换弹操作域 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBMagazineOperation final : public UBBBItemOperation
{
    GENERATED_BODY()

public:
    /**
     * 创建并初始化弹匣运行数据
     * @param Outer     运行数据生命周期所有者
     * @return 创建完成的弹匣运行数据
     */
    UBBBMagazineRuntimeData *InitializeRuntimeData(UObject &Outer) const;

    /**
     * 判断当前弹匣是否允许开火
     * @param RuntimeData   弹匣运行数据
     * @return 是否允许消耗弹药
     */
    bool CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 消耗一发弹药
     * @param RuntimeData   弹匣运行数据
     */
    void ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 响应一次换弹操作
     * @param EquipmentActor    装备实体
     * @param RuntimeData       弹匣运行数据
     * @return 是否成功开始换弹
     */
    bool Reload(ABBBEquipmentActor &EquipmentActor, UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 响应一次远端换弹表现
     * @param EquipmentActor    装备实体
     */
    void PresentReload(ABBBEquipmentActor &EquipmentActor) const;

    /**
     * 更新换弹过程
     * @param EquipmentActor    装备实体
     * @param RuntimeData       弹匣运行数据
     */
    void Update(ABBBEquipmentActor &EquipmentActor, UBBBMagazineRuntimeData &RuntimeData) const;

    /** 弹匣容量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine", meta = (ClampMin = "1"))
    int32 MagazineSize = 30;

    /** 初始备用弹药 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine", meta = (ClampMin = "0"))
    int32 InitialReserveAmmo = 90;

    /** 换弹动画 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine")
    TObjectPtr<UAnimMontage> ReloadMontage = nullptr;
};
