#pragma once
#include "CoreMinimal.h"
class UStaticMesh;
class UNiagaraSystem;
class USoundBase;
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

    /** 爆炸效果的整体缩放倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "爆炸特效缩放", ToolTip = "爆炸效果的整体缩放倍率", ClampMin = "0.01"))
    float EffectScale = 1.0f;
};
