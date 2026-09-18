#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterControlInput.generated.h"
struct FBBBCharacterRuntimeData;

/** 外部控制源提交的世界空间控制事实 */
USTRUCT(BlueprintType)
struct FBBBCharacterControlInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector MoveWorld = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator FacingWorld = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector AimTargetWorld = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAim = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFire = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bWalk = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSprint = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCrouch = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bJump = false;

    /** @return 当前是否允许冲刺 */
    static bool AllowsSprint(bool bAiming);

    /** @return 控制事实是否有效 */
    bool CanApply(const FBBBCharacterRuntimeData &Data) const;

    /** @param Data 角色黑板 */
    void Apply(FBBBCharacterRuntimeData &Data) const;

    /** @param Data 角色黑板 */
    static void FinishFrame(FBBBCharacterRuntimeData &Data);
};
