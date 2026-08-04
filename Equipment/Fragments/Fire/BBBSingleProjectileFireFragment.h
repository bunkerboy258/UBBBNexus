#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBSingleProjectileFireFragment.generated.h"

class ABBBBulletActor;
class ABBBEquipmentPresentationActor;
class FBBBCharacterExternalAPI;
class UAnimMontage;
class UBBBFireRuntimeData;
class USoundBase;

/** 单投射物开火领域 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBSingleProjectileFireFragment final : public UObject
{
    GENERATED_BODY()

public:
    /** 构造单投射物开火配置 */
    UBBBSingleProjectileFireFragment();

    /**
     * 创建开火能力运行数据
     * @param Outer	运行数据所有者
     * @return 创建完成的开火运行数据
     */
    UBBBFireRuntimeData *InitializeRuntimeData(UObject &Outer) const;

    /**
     * 执行一次开火
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData		开火运行数据
     * @return 是否成功开火
     */
    bool Fire(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBFireRuntimeData &RuntimeData) const;

    /**
     * 播放一次远端开火表现
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     */
    void Present(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor) const;

    /** 最短开火间隔 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.01"))
    float FireInterval = 0.2f;

    /** 投射物类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire")
    TSubclassOf<ABBBBulletActor> BulletActorClass;

    /** 投射物初速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.01"))
    float MuzzleSpeed = 30000.0f;

    /** 枪口插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire")
    FName MuzzleSocketName = TEXT("Muzzle");

    /** 开火动画 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire")
    TObjectPtr<UAnimMontage> FireMontage = nullptr;

    /** 开火音效 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire")
    TObjectPtr<USoundBase> FireSound = nullptr;

    /** 垂直后坐力基础值 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.0"))
    float VerticalRecoilAmount = 1.0f;

    /** 水平后坐力基础值 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.0"))
    float HorizontalRecoilAmount = 0.0f;

    /** 垂直后坐力随机范围 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.0"))
    float VerticalRecoilRandom = 0.2f;

    /** 水平后坐力随机范围 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.0"))
    float HorizontalRecoilRandom = 0.4f;

    /** 后坐力恢复速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.01"))
    float RecoilRecoverySpeed = 8.0f;
};
