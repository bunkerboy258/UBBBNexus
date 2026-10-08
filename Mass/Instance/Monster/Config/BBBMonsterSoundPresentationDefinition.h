#pragma once

#include "Engine/DataAsset.h"
#include "BBBMonsterSoundVoice.h"
#include "BBBMonsterSoundPresentationDefinition.generated.h"

class USoundAttenuation;
class USoundConcurrency;

/** 小怪声音的静态素材和发声预算 不产生玩法事实 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterSoundPresentationDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 一代实体始终使用同一声线 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (DisplayName = "声线列表"))
    TArray<FBBBMonsterSoundVoice> Voices;

    /** 所有声音使用的空间衰减 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (DisplayName = "距离衰减"))
    TObjectPtr<USoundAttenuation> Attenuation;

    /** 待机 追击与爬行共享的环境发声上限 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (DisplayName = "环境发声并发"))
    TObjectPtr<USoundConcurrency> AmbientConcurrency;

    /** 攻击 受击 警觉与死亡共享的动作发声上限 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (DisplayName = "动作发声并发"))
    TObjectPtr<USoundConcurrency> ActionConcurrency;

    /** 超出听众范围仍消费事实 但不创建发声 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "1.0", Units = "cm", DisplayName = "最大听觉距离"))
    float AudibleDistance = 2500.0f;

    /** 待机巡逻发声最短间隔 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.1", Units = "s", DisplayName = "待机最短间隔"))
    float IdleIntervalMin = 6.0f;

    /** 待机巡逻发声最长间隔 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.1", Units = "s", DisplayName = "待机最长间隔"))
    float IdleIntervalMax = 12.0f;

    /** 奔跑追击发声最短间隔 冲刺适度缩短 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.1", Units = "s", DisplayName = "追击最短间隔"))
    float ChaseIntervalMin = 2.5f;

    /** 奔跑追击发声最长间隔 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.1", Units = "s", DisplayName = "追击最长间隔"))
    float ChaseIntervalMax = 5.0f;

    /** 爬行发声最短间隔 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.1", Units = "s", DisplayName = "爬行最短间隔"))
    float CrawlIntervalMin = 3.0f;

    /** 爬行发声最长间隔 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.1", Units = "s", DisplayName = "爬行最长间隔"))
    float CrawlIntervalMax = 6.0f;

    /** 连续命中只保留当前表现 不排队补播 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.01", Units = "s", DisplayName = "受击发声最短间隔"))
    float HitInterval = 0.22f;

    /** 过期命中编号被消费但不发声 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.01", Units = "s", DisplayName = "命中表现有效时间"))
    float HitMaxAge = 0.25f;

    /** 单体固定音调下界 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.5", ClampMax = "2.0", DisplayName = "最低音调"))
    float PitchMin = 0.94f;

    /** 单体固定音调上界 */
    UPROPERTY(EditAnywhere, Category = "小怪|声音", meta = (ClampMin = "0.5", ClampMax = "2.0", DisplayName = "最高音调"))
    float PitchMax = 1.06f;

    /** @return 参数和非循环素材是否完整有效 */
    bool IsValid() const;
};
