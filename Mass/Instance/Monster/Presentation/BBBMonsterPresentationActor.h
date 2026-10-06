#pragma once

#include "GameFramework/Actor.h"
#include "BBBMonsterPresentationActor.generated.h"

class USkeletalMeshComponent;
class UBBBMonsterPresentationComponent;
class UBBBMonsterHitReactionComponent;
class UPhysicalAnimationComponent;

/** 仅承载小怪模型和动画的临时表现对象 */
UCLASS(BlueprintType)
class ABBB_EVAC_API ABBBMonsterPresentationActor : public AActor
{
    GENERATED_BODY()

public:
    /** 创建不参与玩法碰撞的表现对象 */
    ABBBMonsterPresentationActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /** @return 小怪骨骼网格 */
    UFUNCTION(BlueprintPure, Category = "BBB|小怪")
    USkeletalMeshComponent* GetMonsterMesh() const;

    /** @return 小怪动画桥接组件 */
    UFUNCTION(BlueprintPure, Category = "BBB|小怪")
    UBBBMonsterPresentationComponent* GetMonsterPresentation() const;

private:
    /** 局部物理回弹表现 */
    UPROPERTY(VisibleAnywhere, Category = "小怪|受击", meta = (DisplayName = "局部受击组件"))
    TObjectPtr<UBBBMonsterHitReactionComponent> HitReaction;

    /** 骨骼恢复驱动 */
    UPROPERTY(VisibleAnywhere, Category = "小怪|受击", meta = (DisplayName = "骨骼恢复组件"))
    TObjectPtr<UPhysicalAnimationComponent> PhysicalAnimation;

    /** 骨骼网格 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (AllowPrivateAccess = "true", DisplayName = "小怪骨骼网格"))
    TObjectPtr<USkeletalMeshComponent> MonsterMesh;

    /** 动画桥接 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (AllowPrivateAccess = "true", DisplayName = "小怪表现组件"))
    TObjectPtr<UBBBMonsterPresentationComponent> MonsterPresentation;
};
