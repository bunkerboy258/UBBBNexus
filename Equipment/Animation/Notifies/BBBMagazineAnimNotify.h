#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BBBMagazineAnimNotify.generated.h"

/** 弹匣动画通知动作 */
UENUM(BlueprintType)
enum class EBBBMagazineAnimNotifyAction : uint8
{
    /** 拔出并掉落当前弹匣 */
    Remove,

    /** 生成并装入新弹匣 */
    Spawn
};

/**
 * 把蒙太奇中的弹匣操作时机交给当前装备
 */
UCLASS(meta = (DisplayName = "BBB Magazine Action"))
class ABBB_EVAC_API UBBBMagazineAnimNotify final : public UAnimNotify
{
    GENERATED_BODY()

public:
    /**
     * 转发弹匣动画通知
     * @param MeshComp          播放动画的角色网格
     * @param Animation         触发通知的动画资源
     * @param EventReference    当前通知事件引用
     */
    virtual void Notify(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;

    /** @return 编辑器中显示的通知名称 */
    virtual FString GetNotifyName_Implementation() const override;

    /** 当前通知触发的弹匣动作 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Equipment|Magazine")
    EBBBMagazineAnimNotifyAction Action = EBBBMagazineAnimNotifyAction::Remove;
};
