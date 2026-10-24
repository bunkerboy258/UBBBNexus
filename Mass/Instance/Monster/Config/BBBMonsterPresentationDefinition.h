#pragma once

#include "Engine/DataAsset.h"
#include "BBBMonsterHitPresentation.h"
#include "BBBMonsterAnimationPresentation.h"
#include "BBBMonsterGibPresentation.h"
#include "BBBMonsterPresentationDefinition.generated.h"

class UBBBMonsterGoreDefinition;
class UBBBMonsterBloodPresentationDefinition;
class UBBBMonsterSoundPresentationDefinition;
class UPhysicsAsset;
class UAnimInstance;

/** 僵尸唯一的表现配置入口 所有分区保存在此资产内部 */
UCLASS(BlueprintType, meta = (DisplayName = "僵尸表现配置"))
class ABBB_EVAC_API UBBBMonsterPresentationDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    UBBBMonsterPresentationDefinition();

    UPROPERTY(EditAnywhere, Instanced, Category = "身体与伤口", meta = (DisplayName = "身体与伤口"))
    TObjectPtr<UBBBMonsterGoreDefinition> Gore;

    UPROPERTY(EditAnywhere, Category = "身体与伤口", meta = (DisplayName = "身体物理资产"))
    TObjectPtr<UPhysicsAsset> PhysicsAsset;

    UPROPERTY(EditAnywhere, Category = "动画", meta = (DisplayName = "正式动画蓝图"))
    TSubclassOf<UAnimInstance> AnimationClass;

    UPROPERTY(EditAnywhere, Category = "身体与伤口", meta = (DisplayName = "相对身体变换"))
    FTransform MeshTransform = FTransform(FRotator(0, -90, 0), FVector(0, 0, -90));

    UPROPERTY(EditAnywhere, Category = "动画", meta = (DisplayName = "动画采样"))
    FBBBMonsterAnimationPresentation Animation;

    UPROPERTY(EditAnywhere, Category = "程序化受击", meta = (DisplayName = "程序化受击"))
    FBBBMonsterHitPresentation HitReaction;

    UPROPERTY(EditAnywhere, Instanced, Category = "血液", meta = (DisplayName = "喷血与环境残留"))
    TObjectPtr<UBBBMonsterBloodPresentationDefinition> Blood;

    UPROPERTY(EditAnywhere, Category = "残肢", meta = (DisplayName = "残肢运动与残留"))
    FBBBMonsterGibPresentation Gibs;

    UPROPERTY(EditAnywhere, Instanced, Category = "声音", meta = (DisplayName = "声音"))
    TObjectPtr<UBBBMonsterSoundPresentationDefinition> Sounds;

    /** @return 所有分区与身体引用是否有效 */
    bool IsValid() const;
};
