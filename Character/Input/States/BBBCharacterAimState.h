#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterAimState.generated.h"

struct FBBBCharacterControlFacts;

/** 瞄准持续状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBCharacterAimState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector AimTargetWorld = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAim = false;

    /** @return 状态内容是否合法 */
    bool IsValid() const
    {
        return !AimTargetWorld.ContainsNaN();
    }

    /**
     * 将状态写入控制事实基座
     * @param Control	待写入的控制事实
     */
    void Apply(FBBBCharacterControlFacts &Control) const;
};
