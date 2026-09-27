#pragma once

#include "GameFramework/Actor.h"
#include "BBBMonsterPresentationActor.generated.h"

class USkeletalMeshComponent;
class UBBBMonsterPresentationComponent;

/** 仅承载小怪模型和动画的临时表现对象 */
UCLASS(BlueprintType)
class ABBB_EVAC_API ABBBMonsterPresentationActor final : public AActor
{
    GENERATED_BODY()

public:
    /** 创建不参与玩法碰撞的表现对象 */
    ABBBMonsterPresentationActor();

    /** @return 小怪骨骼网格 */
    UFUNCTION(BlueprintPure, Category = "BBB|Monster")
    USkeletalMeshComponent* GetMonsterMesh() const;

    /** @return 小怪动画桥接组件 */
    UFUNCTION(BlueprintPure, Category = "BBB|Monster")
    UBBBMonsterPresentationComponent* GetMonsterPresentation() const;

private:
    /** 骨骼网格 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> MonsterMesh;

    /** 动画桥接 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBMonsterPresentationComponent> MonsterPresentation;
};
