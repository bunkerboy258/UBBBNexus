#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BBBCharacterMovementComponent.generated.h"

/**
 * 承接UE角色移动生命周期并显式通知角色完成移动后更新
 */
UCLASS()
class ABBB_EVAC_API UBBBCharacterMovementComponent final : public UCharacterMovementComponent
{
    GENERATED_BODY()

protected:

    /**
     * 在UE完成本帧角色移动后通知所属角色
     * @param DeltaSeconds 本次移动更新时间
     * @param OldLocation  移动前角色位置
     * @param OldVelocity  移动前角色速度
     */
    virtual void OnMovementUpdated(
        float DeltaSeconds,
        const FVector &OldLocation,
        const FVector &OldVelocity) override;
};
