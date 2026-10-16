#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterSoundDisplayAnimNotifyState.generated.h"

class USoundBase;
class USoundAttenuation;
class UAudioComponent;

/** 角色动作通知在实际骨骼位置播放声音 只维护本次表现组件 */
UCLASS(meta = (DisplayName = "BBB 角色动作声音"))
class ABBB_EVAC_API UBBBCharacterSoundDisplayAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

  public:
    /** 本次通知播放的声音 */
    UPROPERTY(EditAnywhere, Category = "BBB|角色|声音", meta = (DisplayName = "声音资源"))
    TObjectPtr<USoundBase> Sound;
    /** 声音来源骨骼 留空时使用网格原点 */
    UPROPERTY(EditAnywhere, Category = "BBB|角色|声音", meta = (DisplayName = "声音挂点", AnimNotifyBoneName = "true"))
    FName SocketName = NAME_None;
    /** 使用距离衰减控制远处动作声音 */
    UPROPERTY(EditAnywhere, Category = "BBB|角色|声音", meta = (DisplayName = "距离衰减"))
    TObjectPtr<USoundAttenuation> Attenuation;
    /** 声音音量倍率 */
    UPROPERTY(EditAnywhere, Category = "BBB|角色|声音", meta = (DisplayName = "音量", ClampMin = "0"))
    float Volume = 1.0f;
    /** 声音音高倍率 */
    UPROPERTY(EditAnywhere, Category = "BBB|角色|声音", meta = (DisplayName = "音高", ClampMin = "0.1"))
    float Pitch = 1.0f;
    /** 通知结束或被打断时停止持续声音 */
    UPROPERTY(EditAnywhere, Category = "BBB|角色|声音", meta = (DisplayName = "结束时停止"))
    bool bStopOnEnd = false;

    /**
     * 在角色骨骼位置生成本次声音
     * @param MeshComp	播放角色动作的网格
     * @param Animation	动作资源
     * @param TotalDuration	通知区间长度
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, float TotalDuration,
                             const FAnimNotifyEventReference &EventReference) override;
    /**
     * 清理当前通知持有的声音引用
     * @param MeshComp	播放角色动作的网格
     * @param Animation	动作资源
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
                           const FAnimNotifyEventReference &EventReference) override;

  private:
    /** 每个播放网格只关联本通知产生的声音 */
    TMap<TWeakObjectPtr<USkeletalMeshComponent>, TWeakObjectPtr<UAudioComponent>> Playing;
};
