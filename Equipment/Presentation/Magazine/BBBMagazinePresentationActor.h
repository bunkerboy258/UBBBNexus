#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBMagazinePresentationActor.generated.h"

class UStaticMeshComponent;

/**
 * 弹匣在虚幻世界中的纯表现实体
 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBMagazinePresentationActor : public AActor
{
    GENERATED_BODY()

public:
    /** 构造弹匣表现组件 */
    ABBBMagazinePresentationActor();

private:
    friend struct FBBBMagazineFragment;

    /** 准备作为武器上的装载弹匣 */
    void PrepareForAttachment();

    /**
     * 让弹匣脱离武器并进入物理掉落
     * @param LifeSeconds    掉落后的保留时长
     */
    void Drop(float LifeSeconds);

    /** 弹匣显示网格 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment|Magazine", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> MagazineMesh;
};
