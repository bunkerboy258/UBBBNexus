#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipmentPresentationActor.generated.h"

class UArrowComponent;
class USceneComponent;

/** 装备在虚幻世界中的纯表现实体 */
UCLASS()
class ABBB_EVAC_API ABBBEquipmentPresentationActor : public AActor
{
    GENERATED_BODY()

public:
    /** 构造装备表现组件 */
    ABBBEquipmentPresentationActor();

    /** @return 装备姿态与插槽使用的组件 */
    virtual USceneComponent *GetEquipmentAttachmentComponent() const;

    /** 设置枪口与瞄准来源共用插槽 */
    void SetMuzzleSocketName(FName InSocketName);

    /** @return 枪口与瞄准来源共用插槽 */
    FName GetMuzzleSocketName() const;

    /** 提交通知到装备动画实例 */
    virtual void NotifyFire(float WorldTimeSeconds);

    /** 提交换弹开始通知到装备动画实例 */
    virtual void NotifyReloadStarted(float WorldTimeSeconds, float DurationSeconds);

    /** 提交换弹完成通知到装备动画实例 */
    virtual void NotifyReloadCompleted(float WorldTimeSeconds);

private:
    /** 装备组件根 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> EquipmentRoot;

    /** 枪口与瞄准来源共用插槽 */
    FName MuzzleSocketName = NAME_None;
};
