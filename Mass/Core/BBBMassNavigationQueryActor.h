#pragma once

#include "GameFramework/Actor.h"
#include "Navigation/APS_MoveToInterface.h"
#include "BBBMassNavigationQueryActor.generated.h"

/** 世界级同步导航参数载体 不保存实体玩法状态 */
UCLASS(Transient, NotPlaceable, NotBlueprintable)
class ABBB_EVAC_API ABBBMassNavigationQueryActor final : public AActor, public IAPS_MoveToInterface
{
    GENERATED_BODY()

public:
    /** 创建不更新不复制的查询载体 */
    ABBBMassNavigationQueryActor();

    /**
     * @param Strength	本次弧线强度
     * @param Points	本次曲线形状参数
     * @param Mode	本次曲线模式
     * @param Offset	本次横向偏移
     * @return 无返回值
     */
    void SetParameters(float Strength, int32 Points, EAPSArcMidpointMode Mode, float Offset);

    /** @return 无返回值 清除当次同步查询参数 */
    void ResetParameters();

    //~ Begin IAPS_MoveToInterface Interface
    /**
     * @param ArcStrength	输出弧线强度
     * @param NumMidPoints	输出曲线形状参数
     * @param MidpointMode	输出曲线模式
     * @param LaneOffset	输出横向偏移
     * @return 无返回值
     */
    virtual void GetMoveToParameters_Implementation(float& ArcStrength, int32& NumMidPoints,
        EAPSArcMidpointMode& MidpointMode, float& LaneOffset) const override;

    /**
     * @param StartLocation	查询起点
     * @param EndLocation	查询终点
     * @param OutPath	输出路径 始终留给 APS 生成
     * @param ArcStrength	插件提供的输入强度
     * @param World	查询世界
     * @param bUseCustomPath	输出是否覆盖路线
     * @param bFound	输出是否提供自定义路线
     * @return 无返回值
     */
    virtual void FindCustomPath_Implementation(const FVector& StartLocation, const FVector& EndLocation,
        TArray<FVector>& OutPath, float ArcStrength, UWorld* World, bool& bUseCustomPath, bool& bFound) const override;
    //~ End IAPS_MoveToInterface Interface

private:
    /** 只在当次同步查询中有效的弧度 */
    float Strength = 0.0f;

    /** 只在当次同步查询中有效的形状参数 */
    int32 Points = 0;

    /** 只在当次同步查询中有效的曲线模式 */
    EAPSArcMidpointMode Mode = EAPSArcMidpointMode::SmoothCurve;

    /** 只在当次同步查询中有效的横向偏移 */
    float Offset = 0.0f;
};
