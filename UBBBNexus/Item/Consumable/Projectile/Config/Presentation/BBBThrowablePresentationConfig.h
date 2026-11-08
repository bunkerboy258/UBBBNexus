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

    /** 沿释放后的实际运动生成短尾迹 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物",
              meta = (DisplayName = "飞行尾迹", ToolTip = "附着于物理投掷物的本地 Niagara 尾迹 爆炸与停稳后停止生成"))
    TObjectPtr<UNiagaraSystem> TrailEffect = nullptr;

    /** 硬质地面弹跳声音 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物",
              meta = (DisplayName = "弹跳音效", ToolTip = "房主确认真实碰撞后各端播放一次 声音强弱由碰撞冲量决定"))
    TObjectPtr<USoundBase> BounceSound = nullptr;

    /** 弹跳声音的空间衰减 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物",
              meta = (DisplayName = "弹跳声音衰减", ToolTip = "控制弹跳声音的近距离传播范围"))
    TObjectPtr<USoundAttenuation> BounceAttenuation = nullptr;

    /** 爆炸效果的整体缩放倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "爆炸特效缩放", ToolTip = "爆炸效果的整体缩放倍率", ClampMin = "0.01"))
    float EffectScale = 1.0f;
};
