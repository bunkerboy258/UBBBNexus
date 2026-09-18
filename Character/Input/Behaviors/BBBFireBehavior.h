#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBBehaviorPolicy.h"
#include "BBBFireBehavior.generated.h"

struct FBBBCharacterRuntimeData;

/** 本帧开火请求 持续按住时每帧提交一次 */
USTRUCT(BlueprintType)
struct FBBBFireInput
{
    GENERATED_BODY()
};

/** 开火行为的固定规则 */
struct FBBBFireBehavior
{
    using FInput = FBBBFireInput;
    static constexpr FBBBBehaviorPolicy Policy{
        EBBBBehaviorDelivery::Request, 300,
        static_cast<uint8>(BBBBehaviorGroup::Reload | BBBBehaviorGroup::Equip), 0};
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};
