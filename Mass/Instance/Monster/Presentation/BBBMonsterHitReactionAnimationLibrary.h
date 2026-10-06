#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterHitReactionAnimationLibrary.generated.h"

/** 动画图使用的纯姿势计算 不读写玩法对象 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterHitReactionAnimationLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * @param Direction	网格组件空间受力方向
     * @param Age	最近命中之后秒数
     * @param Region	命中部位
     * @param BoneRegion	待旋转骨骼对应部位
     * @param bAlive	是否允许活体姿势反馈
     * @return 有界的附加旋转
     */
    UFUNCTION(BlueprintPure, Category = "小怪|受击", meta = (BlueprintThreadSafe, DisplayName = "计算受击骨骼旋转"))
    static FRotator CalculateHitBoneRotation(FVector Direction, float Age, int32 Region, int32 BoneRegion, bool bAlive);
};
