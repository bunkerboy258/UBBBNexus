#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBRifleNewMagazineAnimNotifyState.generated.h"

class UStaticMesh;

/** 在角色换弹动画中管理新弹匣从拿起到装入的表现 */
UCLASS(meta = (DisplayName = "BBB Rifle New Magazine"))
class ABBB_EVAC_API UBBBRifleNewMagazineAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 新弹匣的独立网格 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Rifle|Magazine")
    TObjectPtr<UStaticMesh> MagazineMesh = nullptr;

    /** 握持新弹匣的角色手部骨骼 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Rifle|Magazine")
    FName HandBoneName = TEXT("hand_l");

    /** 新弹匣相对手部骨骼的握持变换 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Rifle|Magazine")
    FTransform HandMagazineTransform = FTransform::Identity;

    /**
     * 按通知实例配置在角色手部生成新弹匣
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
     * 在装入帧或中断时清理手持弹匣
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
