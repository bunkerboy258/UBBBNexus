#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBMagazineFragment.generated.h"

class ABBBEquipmentPresentationActor;
class FBBBCharacterExternalAPI;
class UAnimMontage;
class UBBBMagazineRuntimeData;

/** 弹匣与换弹领域 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBMagazineFragment final : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 创建并初始化弹匣运行数据
     * @param Outer	运行数据生命周期所有者
     * @return 创建完成的弹匣运行数据
     */
    UBBBMagazineRuntimeData *InitializeRuntimeData(UObject &Outer) const;

    /**
     * 判断当前弹匣是否允许开火
     * @param RuntimeData	弹匣运行数据
     * @return 是否允许消耗弹药
     */
    bool CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 消耗一发弹药
     * @param RuntimeData	弹匣运行数据
     */
    void ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 响应一次本地换弹
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData		弹匣运行数据
     * @return 是否成功开始换弹
     */
    bool Reload(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 响应一次远端换弹表现
     * @param CharacterAPI	角色能力接口
     */
    void PresentReload(FBBBCharacterExternalAPI &CharacterAPI) const;

    /**
     * 更新换弹过程
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData		弹匣运行数据
     */
    void Update(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBMagazineRuntimeData &RuntimeData) const;

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
