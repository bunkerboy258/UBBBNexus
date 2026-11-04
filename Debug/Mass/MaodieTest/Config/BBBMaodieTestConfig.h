#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Config/BBBMaodieTestAppearanceConfig.h"
#include "BBBMaodieTestConfig.generated.h"
class UBBBItemDefinition;
class USoundBase;
class UAnimSequence;
/** 耄耋测试行为配置 */
USTRUCT(BlueprintType)
struct FBBBMaodieTestConfig final
{
    GENERATED_BODY()

    /** 本关卡探索编号 零表示不参与十只耄耋探索 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋|探索", meta = (DisplayName = "探索编号", ClampMin = "0", ClampMax = "10", ToolTip = "正式地图的十只耄耋分别配置一至十 同一编号对每位玩家只计数一次"))
    int32 DiscoveryIndex = 0;

    /** 专用灯光与橙色轮廓配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋|外观", meta = (DisplayName = "外观配置"))
    FBBBMaodieTestAppearanceConfig Appearance;
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
    float ThrowSpeedMin = 400.0f;
    /** 水平速度上限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "水平速度上限", ClampMin = "0"))
    float ThrowSpeedMax = 550.0f;
    /** 向上速度下限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "向上速度下限", ClampMin = "0"))
    float ThrowUpSpeedMin = 400.0f;
    /** 向上速度上限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "向上速度上限", ClampMin = "0"))
    float ThrowUpSpeedMax = 550.0f;
    /** 出手高度下限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "出手高度下限", ClampMin = "0"))
    float ThrowHeightMin = 45.0f;
    /** 出手高度上限 单位厘米或厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "出手高度上限", ClampMin = "0"))
    float ThrowHeightMax = 60.0f;
    /** 每位玩家投掷上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "每位玩家投掷上限", ClampMin = "0"))
    int32 ThrowLimit = 8;

    /** 距离超过此厘米值才开始跟随 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "开始跟随距离", ClampMin = "1"))
    float FollowStartDistance = 400.0f;
    /** 距离小于此厘米值时停步等待 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "停止跟随距离", ClampMin = "1"))
    float FollowStopDistance = 250.0f;
    /** 跟随时的最大行走速度 单位厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "跟随速度", ClampMin = "1"))
    float FollowSpeed = 180.0f;
    /** 头部相对身体的最大偏航角 单位度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "头部左右转动上限", ClampMin = "1"))
    float HeadYawLimit = 60.0f;
    /** 头部相对身体的最大俯仰角 单位度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "头部上下转动上限", ClampMin = "1"))
    float HeadPitchLimit = 35.0f;
    /** 注视与回正使用的旋转插值速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "头部转动平滑速度", ClampMin = "0.1"))
    float HeadTurnSpeed = 6.0f;

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
