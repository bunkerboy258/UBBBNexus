#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterFootstepPresentationAnimNotifyState.generated.h"

/** 脚步表现通知的状态类型边界 具体蓝图在 Begin 复用 GASP Foley 触发链 */
UCLASS(Blueprintable, meta = (DisplayName = "Character Footstep"))
class ABBB_EVAC_API UBBBCharacterFootstepPresentationAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
};
