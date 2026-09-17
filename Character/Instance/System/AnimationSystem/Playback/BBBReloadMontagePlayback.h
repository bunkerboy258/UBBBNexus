#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBReloadMontagePlayback.generated.h"
class ABBBCharacter;

/** 保存每次换弹播放身份并转发标准蒙太奇通知 */
UCLASS()
class UBBBReloadMontagePlayback final : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 开始一次绑定操作序号的播放
     * @param AnimInstance	角色动画实例
     * @param Montage	换弹蒙太奇
     * @param PlayRate	播放倍率
     * @param InSequence	换弹序号
     * @return 无
     */
    void Start(UAnimInstance &AnimInstance, UAnimMontage &Montage, float PlayRate, int32 InSequence);

    /**
     * 停止指定序号的播放
     * @param InSequence	待取消的操作序号
     * @return 无
     */
    void Cancel(int32 InSequence);

    /** @return 回调对象是否可以释放 */
    bool IsFinished();

private:
    UFUNCTION()
    void OnNotify(FName Name, const FBranchingPointNotifyPayload &Payload);
    void OnEnded(UAnimMontage *Montage, bool bInterrupted);
    void OnBlendingOut(UAnimMontage *Montage, bool bInterrupted);
    void Finish(bool bInterrupted);
    void Unbind();

    TWeakObjectPtr<ABBBCharacter> Character;
    TWeakObjectPtr<UAnimInstance> Animation;
    TWeakObjectPtr<UAnimMontage> PlayedMontage;
    int32 Sequence = INDEX_NONE;
    int32 MontageInstanceId = INDEX_NONE;
    bool bFinished = false;
    bool bStartReported = false;
    bool bEndReported = false;
};
