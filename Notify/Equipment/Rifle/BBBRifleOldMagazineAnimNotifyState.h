#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBRifleOldMagazineAnimNotifyState.generated.h"

class UStaticMesh;

/** 在武器换弹动画中管理旧弹匣从拔出到掉落的表现 */
UCLASS(meta = (DisplayName = "BBB Rifle Old Magazine"))
class ABBB_EVAC_API UBBBRifleOldMagazineAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 旧弹匣的独立网格 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Rifle|Magazine")
    TObjectPtr<UStaticMesh> MagazineMesh = nullptr;

    /** 武器网格中需要隐藏的弹匣骨骼 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Rifle|Magazine")
    FName WeaponMagazineBoneName = TEXT("Magazine_joint");

    /** 抓取旧弹匣的角色手部骨骼 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Rifle|Magazine")
    FName HandBoneName = TEXT("hand_l");

    /** 旧弹匣相对手部骨骼的握持变换 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Rifle|Magazine")
    FTransform HandMagazineTransform = FTransform::Identity;

    /** 从通知区间起点到甩出旧弹匣的动画时间 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Rifle|Magazine", meta = (ClampMin = "0.0"))
    float ReleaseDelaySeconds = 0.166667f;

    /**
     * 从武器转交旧弹匣到角色左手
     * @param MeshComp		播放武器动画的网格
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
     * 在实例指定帧把旧弹匣甩到世界中
     * @param MeshComp		播放武器动画的网格
     * @param Animation	动画资产
     * @param FrameDeltaTime	本帧动画时间
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyTick(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        float FrameDeltaTime,
        const FAnimNotifyEventReference &EventReference) override;

    /**
     * 在动画结束或中断时收束旧弹匣表现
     * @param MeshComp		播放武器动画的网格
     * @param Animation	动画资产
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyEnd(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;
};
