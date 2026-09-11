#pragma once

#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/MonsterMass/States/MonsterStateData.h"

#include "MonsterPresentationComponent.generated.h"

class UAnimSequenceBase;

/** 向小怪动画蓝图提供只读 Mass 表现状态 */
UCLASS(ClassGroup = "Monster", BlueprintType, meta = (BlueprintSpawnableComponent))
class ABBB_EVAC_API UMonsterPresentationComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UMonsterPresentationComponent();

    /**
     * 写入本帧由 Mass 逻辑生成的表现状态
     * @param InState        小怪权威状态
     * @param InSpeed        当前移动速度
     * @param InStateTime    状态进入世界时间
     */
    void ApplyPresentationState(
        EMonsterState InState,
        float InSpeed,
        float InStateTime);

    /** @return 当前小怪状态 */
    UFUNCTION(BlueprintPure, Category = "Monster|Presentation")
    EMonsterState GetMonsterState() const;

    /** @return 当前移动速度 */
    UFUNCTION(BlueprintPure, Category = "Monster|Presentation")
    float GetMovementSpeed() const;

    /** @return 当前状态进入时间 */
    UFUNCTION(BlueprintPure, Category = "Monster|Presentation")
    float GetStateEnteredTime() const;

private:
    /** 根据逻辑状态选择表现动画 */
    UAnimSequenceBase* GetAnimationForState(EMonsterState InState) const;

    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> IdleAnimation;

    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> ScoutAnimation;

    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> ChaseAnimation;

    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> AttackAnimation;

    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> HurtAnimation;

    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> DeadAnimation;

    UPROPERTY(Transient)
    EMonsterState MonsterState = EMonsterState::Idle;

    UPROPERTY(Transient)
    float MovementSpeed = 0.0f;

    UPROPERTY(Transient)
    float StateEnteredTime = 0.0f;

    EMonsterState LastPlayedState = EMonsterState::Idle;

    bool bHasAppliedAnimation = false;
};
