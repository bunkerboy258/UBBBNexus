#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterSoundVoice.generated.h"

class USoundBase;

/** 同一声线的行为声音池 不承担玩法状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterSoundVoice final
{
    GENERATED_BODY()

    /** 编辑器中的声线名称 */
    UPROPERTY(EditAnywhere, Category = "声音", meta = (DisplayName = "声线名称"))
    FName Name;

    /** 待机与巡逻的间歇低鸣 */
    UPROPERTY(EditAnywhere, Category = "声音", meta = (DisplayName = "待机巡逻"))
    TArray<TObjectPtr<USoundBase>> Idle;

    /** 进入警觉时的短促发声 */
    UPROPERTY(EditAnywhere, Category = "声音", meta = (DisplayName = "警觉"))
    TArray<TObjectPtr<USoundBase>> Alert;

    /** 追击期间的间歇发声 不代替脚步 */
    UPROPERTY(EditAnywhere, Category = "声音", meta = (DisplayName = "追击"))
    TArray<TObjectPtr<USoundBase>> Chase;

    /** 每次成立攻击的发声 */
    UPROPERTY(EditAnywhere, Category = "声音", meta = (DisplayName = "攻击"))
    TArray<TObjectPtr<USoundBase>> Attack;

    /** 实际命中后的短促反应 */
    UPROPERTY(EditAnywhere, Category = "声音", meta = (DisplayName = "受击"))
    TArray<TObjectPtr<USoundBase>> Hit;

    /** 死亡事实首次成立时的发声 */
    UPROPERTY(EditAnywhere, Category = "声音", meta = (DisplayName = "死亡"))
    TArray<TObjectPtr<USoundBase>> Death;

    /** 持续爬行的间歇发声 不代替地面摩擦 */
    UPROPERTY(EditAnywhere, Category = "声音", meta = (DisplayName = "爬行"))
    TArray<TObjectPtr<USoundBase>> Crawl;

    /** 本声线统一增益 */
    UPROPERTY(EditAnywhere, Category = "声音", meta = (ClampMin = "0.01", ClampMax = "4.0", DisplayName = "声线音量"))
    float Volume = 2.0f;
};
