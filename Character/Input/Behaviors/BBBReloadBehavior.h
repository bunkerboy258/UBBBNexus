#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterAnimationInput.h"
#include "BBBReloadBehavior.generated.h"

struct FBBBCharacterRuntimeData;

/** 外部提交的换弹请求 */
USTRUCT(BlueprintType)
struct FBBBReloadInput
{
    GENERATED_BODY()
};

/** 换弹行为的固定规则与跨帧事件处理 */
struct FBBBReloadBehavior
{
    using FInput = FBBBReloadInput;
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
    static void BeginFrame(FBBBCharacterRuntimeData &Data);
    static void OnEquipmentResult(const FBBBEquipmentActionEvent &Event, FBBBCharacterRuntimeData &Data);
    static void OnAnimationEvent(const FBBBCharacterReloadAnimationInput &Event, FBBBCharacterRuntimeData &Data);
    static void Cancel(FBBBCharacterRuntimeData &Data);
};
