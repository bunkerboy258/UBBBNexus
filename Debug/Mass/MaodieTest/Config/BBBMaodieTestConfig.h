#pragma once
#include "CoreMinimal.h"
#include "BBBMaodieTestConfig.generated.h"
class UBBBItemDefinition;
class USoundBase;
class UAnimSequence;
/** 耄耋测试行为配置 */
USTRUCT(BlueprintType)
struct FBBBMaodieTestConfig final
{
    GENERATED_BODY()
    /** 游荡半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "游荡半径", ClampMin = "1"))
    float WanderRadius = 500.0f;
    /** 移动速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "移动速度", ClampMin = "1"))
    float MoveSpeed = 100.0f;
    /** 游荡停留秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "游荡停留秒数", ClampMin = "0.1"))
    float PauseSeconds = 2.0f;
    /** 玩家发现距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "玩家发现距离", ClampMin = "1"))
    float DetectionRadius = 600.0f;
    /** 首次观察秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "首次观察秒数", ClampMin = "0.1"))
    float ObserveSeconds = 1.5f;
    /** 平均投掷频率 单位赫兹 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "平均投掷频率", ClampMin = "0.1"))
    float ThrowFrequency = 2.0f;
    /** 间隔相对平均值的随机比例 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "投掷间隔随机比例", ClampMin = "0", ClampMax = "0.9"))
    float IntervalJitter = 0.2f;
    /** 哈气持续秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "哈气持续秒数", ClampMin = "0.1"))
    float HissDuration = 0.8f;
    /** 丢失目标等待秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "丢失目标等待秒数", ClampMin = "0.1"))
    float LoseInterestDelay = 1.0f;
    /** 看向玩家转向速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "看向玩家转向速度", ClampMin = "0.1"))
    float TurnSpeed = 4.0f;
    /** 水平速度下限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "水平速度下限", ClampMin = "0"))
    float ThrowSpeedMin = 200.0f;
    /** 水平速度上限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "水平速度上限", ClampMin = "0"))
    float ThrowSpeedMax = 300.0f;
    /** 向上速度下限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "向上速度下限", ClampMin = "0"))
    float ThrowUpSpeedMin = 200.0f;
    /** 向上速度上限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "向上速度上限", ClampMin = "0"))
    float ThrowUpSpeedMax = 300.0f;
    /** 出手高度下限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "出手高度下限", ClampMin = "0"))
    float ThrowHeightMin = 30.0f;
    /** 出手高度上限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "出手高度上限", ClampMin = "0"))
    float ThrowHeightMax = 45.0f;
    /** 每位玩家投掷上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "每位玩家投掷上限", ClampMin = "0"))
    int32 ThrowLimit = 5;

    /** 可重复等概率抽取的物品 每次投掷一件 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "随机掉落池"))
    TArray<TObjectPtr<UBBBItemDefinition>> DropPool;
    /** 发现玩家时的哈气音效 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "哈气音效"))
    TObjectPtr<USoundBase> HissSound;
    /** 游荡时循环播放的动画 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "移动动画"))
    TObjectPtr<UAnimSequence> WalkAnimation;
    /** 停留与观察时循环播放的动画 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "待机动画"))
    TObjectPtr<UAnimSequence> IdleAnimation;
};
