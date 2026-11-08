#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterNewMagazineDisplayAnimNotifyState.generated.h"

class UStaticMesh;

/** 在角色换弹动画中生成并清理手持新弹匣 */
UCLASS(meta = (DisplayName = "BBB 角色新弹匣表现"))
class ABBB_EVAC_API UBBBCharacterNewMagazineDisplayAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 新弹匣独立网格 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|角色|弹匣", meta = (DisplayName = "弹匣网格"))
    TObjectPtr<UStaticMesh> MagazineMesh = nullptr;

    /** 握持新弹匣的手部骨骼 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|角色|弹匣", meta = (DisplayName = "手部骨骼名"))
    FName HandBoneName = TEXT("hand_l");

    /** 新弹匣相对手部骨骼的握持变换 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|角色|弹匣", meta = (DisplayName = "手持弹匣变换"))
    FTransform HandMagazineTransform = FTransform::Identity;

    /**
     * 在角色手上直接生成新弹匣表现
     * @param MeshComp		播放角色动画的网格
     * @param Animation	动画资产
     * @param TotalDuration	通知区间长度
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyBegin(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        float TotalDuration,
        const FAnimNotifyEventReference &EventReference) override;

    /**
     * 在装入动作或动画中断时清理手持表现
     * @param MeshComp		播放角色动画的网格
     * @param Animation	动画资产
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyEnd(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;
};
