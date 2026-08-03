
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/Base/BBBFireFragment.h"
#include "BBBSingleProjectileFireFragment.generated.h"
class ABBBBulletActor;
class ABBBWeaponActor;
class UAnimMontage;
class UBBBFireRuntimeData;
class UBBBItemFragmentRuntimeData;
class USoundBase;
struct FBBBShotEvent;

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBSingleProjectileFireFragment final : public UBBBFireFragment
{
    GENERATED_BODY()
public:

    /**
     * 构造 Fragment 默认 Bullet 类型为 ABBBBulletActor
     */
    UBBBSingleProjectileFireFragment();

    /**
     * 校验射速后按枪口插槽方向生成 Bullet 并播放开火表现与后坐力
     * @param Weapon	武器模型
     * @param RuntimeData	开火运行数据
     * @param OutShotEvent	输出射击事件
     * @return 是否成功开火
     */
    virtual bool Fire(ABBBWeaponActor &Weapon, UBBBFireRuntimeData &RuntimeData, FBBBShotEvent &OutShotEvent) const override;

    /**
     * 播放开火 Montage 与音效的表现层反馈
     * @param Weapon	武器模型
     */
    virtual void Present(ABBBWeaponActor &Weapon) const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon", meta = (ClampMin = "0.01"))
    float FireInterval = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Projectile")
    TSubclassOf<ABBBBulletActor> BulletActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Projectile", meta = (ClampMin = "0.01"))
    float MuzzleSpeed = 30000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Socket")
    FName MuzzleSocketName = TEXT("Muzzle");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation")
    TObjectPtr<UAnimMontage> FireMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Audio")
    TObjectPtr<USoundBase> FireSound = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.0"))
    float VerticalRecoilAmount = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.0"))
    float HorizontalRecoilAmount = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.0"))
    float VerticalRecoilRandom = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.0"))
    float HorizontalRecoilRandom = 0.4f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.01"))
    float RecoilRecoverySpeed = 8.0f;
};
