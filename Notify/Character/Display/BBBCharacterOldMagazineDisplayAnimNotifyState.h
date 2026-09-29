#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterOldMagazineDisplayAnimNotifyState.generated.h"

class UStaticMesh;

/** 在角色换弹动画中生成手持旧弹匣并于区间结束时甩出 */
UCLASS(meta = (DisplayName = "BBB Character Old Magazine Display"))
class ABBB_EVAC_API UBBBCharacterOldMagazineDisplayAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 旧弹匣独立网格 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Character|Magazine")
    TObjectPtr<UStaticMesh> MagazineMesh = nullptr;

    /** 握持旧弹匣的手部骨骼 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Character|Magazine")
    FName HandBoneName = TEXT("hand_l");

    /** 旧弹匣相对手部骨骼的握持变换 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Character|Magazine")
    FTransform HandMagazineTransform = FTransform::Identity;

    /** 掉落弹匣的世界存活时间 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Character|Magazine", meta = (ClampMin = "0.01"))
    float DroppedLifeSeconds = 20.0f;

    /**
     * 在角色手上直接生成旧弹匣表现
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
     * 播放到区间终点时甩出旧弹匣 中断时只清理手持表现
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
