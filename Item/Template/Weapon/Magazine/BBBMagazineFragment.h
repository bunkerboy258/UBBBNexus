
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/Fragment/BBBItemFragment.h"
#include "BBBMagazineFragment.generated.h"
class ABBBWeaponActor;
class UAnimMontage;
class UBBBItemFragmentRuntimeData;
class UBBBMagazineRuntimeData;

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBMagazineFragment final : public UBBBItemFragment
{
    GENERATED_BODY()
public:

    /**
     * 创建弹匣运行数据并按配置初始化弹药
     * @param Outer	运行数据生命周期所有者
     * @return 创建的弹匣运行数据
     */
    virtual UBBBItemFragmentRuntimeData *InitializeRuntimeData(UObject &Outer) const override;

    /**
     * 校验是否允许消耗一发弹药
     * @param RuntimeData	弹匣运行数据
     * @return 非装填状态且弹匣有弹药时返回 true
     */
    bool CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 消耗一发弹匣弹药
     * @param RuntimeData	弹匣运行数据
     */
    void ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 校验条件后开始装填并播放装填 Montage
     * @param Weapon	武器模型
     * @param RuntimeData	弹匣运行数据
     * @return 是否成功开始装填
     */
    bool Reload(ABBBWeaponActor &Weapon, UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 播放装填 Montage 的表现层反馈
     * @param Weapon	武器模型
     */
    void PresentReload(ABBBWeaponActor &Weapon) const;

    /**
     * 每帧更新装填状态并在装填结束时结算弹药
     * @param Weapon	武器模型
     * @param RuntimeData	弹匣运行数据
     */
    void Update(ABBBWeaponActor &Weapon, UBBBMagazineRuntimeData &RuntimeData) const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Ammo", meta = (ClampMin = "1"))
    int32 MagazineSize = 30;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Ammo", meta = (ClampMin = "0"))
    int32 InitialReserveAmmo = 90;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation")
    TObjectPtr<UAnimMontage> ReloadMontage = nullptr;
};
