#pragma once

#include "BBBMonsterHitRotationMode.h"
#include "BBBMonsterHitRegionPresentation.generated.h"

/** 一个部位的施力规则 不改变玩法命中部位 */
USTRUCT(BlueprintType)
struct FBBBMonsterHitRegionPresentation final
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (DisplayName = "施力骨骼"))
    FName Bone = TEXT("spine_01");

    UPROPERTY(EditAnywhere, meta = (DisplayName = "直线冲量", ClampMin = "0", ClampMax = "10000"))
    float LinearImpulse = 1080;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "旋转冲量", ClampMin = "0", ClampMax = "20000"))
    float AngularImpulse = 1960;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "旋转方向"))
    EBBBMonsterHitRotationMode RotationMode = EBBBMonsterHitRotationMode::BodyDeflection;
};
