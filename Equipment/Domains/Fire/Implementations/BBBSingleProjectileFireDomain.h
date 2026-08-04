#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Fire/Base/BBBFireDomain.h"
#include "BBBSingleProjectileFireDomain.generated.h"

class ABBBBulletActor;
class UAnimMontage;
class USoundBase;

/** 单投射物开火领域 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBSingleProjectileFireDomain final : public UBBBFireDomain
{
    GENERATED_BODY()

public:
    /** 构造单投射物开火配置 */
    UBBBSingleProjectileFireDomain();

    //~ Begin UBBBFireDomain Interface
    virtual bool Fire(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBFireRuntimeData &RuntimeData) const override;

    virtual void Present(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor) const override;
    //~ End UBBBFireDomain Interface

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
