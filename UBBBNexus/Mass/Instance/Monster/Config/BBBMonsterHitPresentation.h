#pragma once

#include "BBBMonsterHitRegionPresentation.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterHitPresentation.generated.h"

class UHitReactProfile;

/** 共享的物理受击调节 不追加命中队列 */
USTRUCT(BlueprintType)
struct FBBBMonsterHitPresentation final
{
    GENERATED_BODY()

    FBBBMonsterHitPresentation();

    UPROPERTY(EditAnywhere, meta = (DisplayName = "总力度倍率", ClampMin = "0", ClampMax = "4"))
    float ImpulseScale = 1;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "同骨骼连射力度倍率", ClampMin = "0", ClampMax = "2"))
    float RepeatImpulseScale = 1;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "同骨骼单次冲量上限", ClampMin = "0.1", ClampMax = "4"))
    float MaximumImpulseScale = 4;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "躯干"))
    FBBBMonsterHitRegionPresentation Torso;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "头部"))
    FBBBMonsterHitRegionPresentation Head;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "左臂"))
    FBBBMonsterHitRegionPresentation LeftArm;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "右臂"))
    FBBBMonsterHitRegionPresentation RightArm;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "左腿"))
    FBBBMonsterHitRegionPresentation LeftLeg;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "右腿"))
    FBBBMonsterHitRegionPresentation RightLeg;

    UPROPERTY(EditAnywhere, Instanced, meta = (DisplayName = "躯干物理混合"))
    TObjectPtr<UHitReactProfile> TorsoProfile;

    UPROPERTY(EditAnywhere, Instanced, meta = (DisplayName = "头部物理混合"))
    TObjectPtr<UHitReactProfile> HeadProfile;

    UPROPERTY(EditAnywhere, Instanced, meta = (DisplayName = "手臂物理混合"))
    TObjectPtr<UHitReactProfile> ArmsProfile;

    UPROPERTY(EditAnywhere, Instanced, meta = (DisplayName = "腿部物理混合"))
    TObjectPtr<UHitReactProfile> LegsProfile;

    /** @param Region 当前玩法部位 @return 配对的表现施力规则 */
    const FBBBMonsterHitRegionPresentation& ReadRegion(EBBBMonsterHitRegion Region) const;
};
