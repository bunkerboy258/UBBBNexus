#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBEquipmentSpawnEffectDisplayAnimNotifyState.generated.h"

class UNiagaraSystem;

/** 装备动画在通知开始时生成一次可回池的特效 */
UCLASS(meta = (DisplayName = "BBB 装备生成特效"))
class ABBB_EVAC_API UBBBEquipmentSpawnEffectDisplayAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 生成的一次性 Niagara 特效 留空时跳过 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|特效", meta = (DisplayName = "特效资源", ToolTip = "每次通知开始生成一次 特效必须能够自行播放结束 留空时跳过"))
    TObjectPtr<UNiagaraSystem> Effect = nullptr;

    /** 特效生成使用的装备插槽或骨骼 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|特效", meta = (DisplayName = "生成插槽", AnimNotifyBoneName = "true", ToolTip = "使用当前播放动画的装备网格插槽 留空时使用网格原点"))
    FName SocketName = NAME_None;

    /** 相对于生成插槽的特效变换 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|特效", meta = (DisplayName = "相对变换", ToolTip = "位置与朝向相对于生成插槽 缩放控制特效尺寸"))
    FTransform RelativeTransform = FTransform::Identity;

    /** 特效组件是否持续跟随装备插槽 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|特效", meta = (DisplayName = "跟随插槽", ToolTip = "开启时组件附着在装备上 关闭时留在生成位置 粒子的局部或世界空间由 Niagara 资源决定"))
    bool bAttached = true;

    /**
     * 在通知开始时生成一次特效
     * @param MeshComp		播放装备动画的网格
     * @param Animation		动画资产
     * @param TotalDuration	通知区间长度
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyBegin(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        float TotalDuration,
        const FAnimNotifyEventReference &EventReference) override;
};
