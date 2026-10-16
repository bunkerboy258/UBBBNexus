#pragma once
#include "CoreMinimal.h"
class UStaticMesh;
class UNiagaraSystem;
class USoundBase;
class USoundAttenuation;
#include "BBBThrowablePresentationConfig.generated.h"

/** 投掷物Presentation静态配置 */
USTRUCT(BlueprintType)
struct FBBBThrowablePresentationConfig final
{
    GENERATED_BODY()

    /** 释放后的可见静态网格 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "投掷物网格", ToolTip = "释放后的可见静态网格"))
    TObjectPtr<UStaticMesh> Mesh = nullptr;

    /** 相对物理球体的可见网格变换 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "网格局部变换", ToolTip = "相对物理球体的可见网格变换"))
    FTransform MeshTransform = FTransform::Identity;

    /** 爆炸时在每端生成一次的 Niagara 效果 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "爆炸特效", ToolTip = "爆炸时在每端生成一次的 Niagara 效果"))
    TObjectPtr<UNiagaraSystem> ExplosionEffect = nullptr;

    /** 爆炸时在每端播放一次的声音 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "爆炸音效", ToolTip = "爆炸时在每端播放一次的声音"))
    TObjectPtr<USoundBase> ExplosionSound = nullptr;

    /** 爆炸声音的空间衰减 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物",
              meta = (DisplayName = "爆炸声音衰减", ToolTip = "控制爆炸声音的空间定位与距离衰减"))
    TObjectPtr<USoundAttenuation> ExplosionAttenuation = nullptr;

    /** 硬质地面弹跳声音 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物",
              meta = (DisplayName = "弹跳音效", ToolTip = "物理速度突变时播放的轻量弹跳声音"))
    TObjectPtr<USoundBase> BounceSound = nullptr;

    /** 弹跳声音的空间衰减 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物",
              meta = (DisplayName = "弹跳声音衰减", ToolTip = "控制弹跳声音的近距离传播范围"))
    TObjectPtr<USoundAttenuation> BounceAttenuation = nullptr;

    /** 爆炸效果的整体缩放倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "爆炸特效缩放", ToolTip = "爆炸效果的整体缩放倍率", ClampMin = "0.01"))
    float EffectScale = 1.0f;
};
