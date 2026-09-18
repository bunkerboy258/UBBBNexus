#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBBehaviorPolicy.h"
#include "BBBEquipBehavior.generated.h"

struct FBBBCharacterRuntimeData;

/** 外部提交的切换装备请求 */
USTRUCT(BlueprintType)
struct FBBBEquipInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 EquipSlot = INDEX_NONE;
};

/** 切枪行为的固定规则 */
struct FBBBEquipBehavior
{
    using FInput = FBBBEquipInput;
    static constexpr FBBBBehaviorPolicy Policy{
        EBBBBehaviorDelivery::Request, 500, 0, BBBBehaviorGroup::Reload};
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};
