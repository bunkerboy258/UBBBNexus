#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBBehaviorPolicy.h"
#include "BBBControlBehaviors.generated.h"

struct FBBBCharacterRuntimeData;

/** 世界空间移动输入 */
USTRUCT(BlueprintType)
struct FBBBMoveInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector World = FVector::ZeroVector;
};

/** 世界空间观察事实 */
USTRUCT(BlueprintType)
struct FBBBViewInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator FacingWorld = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector AimTargetWorld = FVector::ZeroVector;
};

/** 持续瞄准输入 */
USTRUCT(BlueprintType)
struct FBBBAimInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHeld = false;
};

/** 持续行走输入 */
USTRUCT(BlueprintType)
struct FBBBWalkInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHeld = false;
};

/** 持续冲刺输入 */
USTRUCT(BlueprintType)
struct FBBBSprintInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHeld = false;
};

/** 持续蹲伏输入 */
USTRUCT(BlueprintType)
struct FBBBCrouchInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHeld = false;
};

/** 单帧跳跃输入 */
USTRUCT(BlueprintType)
struct FBBBJumpInput
{
    GENERATED_BODY()
};

/** 移动输入的固定规则 */
struct FBBBMoveBehavior
{
    using FInput = FBBBMoveInput;
    static constexpr FBBBBehaviorPolicy Policy{EBBBBehaviorDelivery::Snapshot, 700};
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};

/** 观察输入的固定规则 */
struct FBBBViewBehavior
{
    using FInput = FBBBViewInput;
    static constexpr FBBBBehaviorPolicy Policy{EBBBBehaviorDelivery::Snapshot, 700};
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};

/** 瞄准输入的固定规则 */
struct FBBBAimBehavior
{
    using FInput = FBBBAimInput;
    static constexpr FBBBBehaviorPolicy Policy{EBBBBehaviorDelivery::Snapshot, 600};
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};

/** 行走输入的固定规则 */
struct FBBBWalkBehavior
{
    using FInput = FBBBWalkInput;
    static constexpr FBBBBehaviorPolicy Policy{EBBBBehaviorDelivery::Snapshot, 600};
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};

/** 蹲伏输入的固定规则 */
struct FBBBCrouchBehavior
{
    using FInput = FBBBCrouchInput;
    static constexpr FBBBBehaviorPolicy Policy{EBBBBehaviorDelivery::Snapshot, 600};
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};

/** 跳跃输入的固定规则 */
struct FBBBJumpBehavior
{
    using FInput = FBBBJumpInput;
    static constexpr FBBBBehaviorPolicy Policy{EBBBBehaviorDelivery::Request, 550};
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};

/** 冲刺输入的固定规则 */
struct FBBBSprintBehavior
{
    using FInput = FBBBSprintInput;
    static constexpr FBBBBehaviorPolicy Policy{
        EBBBBehaviorDelivery::Snapshot, 100,
        static_cast<uint8>(BBBBehaviorGroup::Aim | BBBBehaviorGroup::Fire), 0};
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};
