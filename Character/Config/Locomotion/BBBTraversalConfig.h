#pragma once

#include "CoreMinimal.h"
#include "BBBTraversalConfig.generated.h"

/** 角色翻越与攀爬的独立几何和执行配置 */
USTRUCT(BlueprintType)
struct FBBBTraversalConfig final
{
    GENERATED_BODY()

    /** 是否在地面跳跃输入时检查翻越 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "启用翻越"))
    bool bEnabled = true;

    /** 向前查找障碍的距离 单位厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "前方探测距离", ClampMin = "1"))
    float ProbeDistance = 120.0f;

    /** 前方障碍探测球半径 单位厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "探测半径", ClampMin = "1"))
    float ProbeRadius = 12.0f;

    /** 可触发翻越的最低障碍高度 单位厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "最低障碍高度", ClampMin = "1"))
    float MinHeight = 140.0f;

    /** 低平台动作允许的最大高度 单位厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "低平台最高高度", ClampMin = "1"))
    float LowMaxHeight = 170.0f;

    /** 高平台动作允许的最大高度 单位厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "最高攀爬高度", ClampMin = "1"))
    float MaxHeight = 215.0f;

    /** 跨越动作允许的最大障碍厚度 单位厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "跨越最大厚度", ClampMin = "1"))
    float VaultMaxDepth = 100.0f;

    /** 沿障碍顶部搜索另一侧边缘的最大距离 单位厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "顶部搜索距离", ClampMin = "1"))
    float TopSearchDistance = 180.0f;

    /** 顶部搜索采样间隔 单位厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "顶部采样间隔", ClampMin = "1"))
    float TopSampleSpacing = 15.0f;

    /** 胶囊与障碍之间保留的检查间隙 单位厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "空间检查间隙", ClampMin = "0"))
    float Clearance = 3.0f;

    /** 动画未能启动时取消翻越的等待时间 单位秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "动画启动超时", ClampMin = "0.1"))
    float StartTimeout = 1.0f;

    /** 翻越卡住时的最长执行时间 单位秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "最长执行时间", ClampMin = "1"))
    float MaxDuration = 7.0f;
};
