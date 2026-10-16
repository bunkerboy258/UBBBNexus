#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionUseState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionInputState.h"
#include "BBBCharacterInteractionDomainState.generated.h"

class FBBBCharacterInteractionSystem;
class FBBBCharacterInteractionSelectionProcessor;
class FBBBCharacterInteractionLifecycleProcessor;
class FBBBCharacterParseSystem;

/** 角色装备领域状态的唯一持有者 */
USTRUCT(BlueprintType)
struct FBBBCharacterInteractionDomainState final
{
    GENERATED_BODY()

public:
    /** @return 当前装备独立使用状态 */
    const FBBBCharacterInteractionUseState &ReadInteractionUseState() const { return InteractionUseState; }
    /** @return 角色装备选择状态 */
    const FBBBCharacterInteractionSelectionState &ReadInteractionSelectionState() const
    {
        return InteractionSelectionState;
    }

    /** @return 等待转交的装备动画通知 */
    const FBBBCharacterInteractionInputState &ReadInteractionInputState() const
    {
        return InteractionInputState;
    }

private:
    friend class FBBBCharacterInteractionVisibilityProcessor;
    friend class FBBBCharacterInteractionActionPermissionProcessor;
    /** 独立使用生命周期 */
    UPROPERTY(Transient)
    FBBBCharacterInteractionUseState InteractionUseState;
    friend class FBBBCharacterInteractionInputProcessor;
    /** 动画通知通信状态 */
    FBBBCharacterInteractionInputState InteractionInputState;

    friend class FBBBCharacterInteractionSystem;
    friend class FBBBCharacterInteractionSelectionProcessor;
    friend class FBBBCharacterInteractionLifecycleProcessor;
    friend class FBBBCharacterParseSystem;

    /** 角色装备选择状态 */
    UPROPERTY()
    FBBBCharacterInteractionSelectionState InteractionSelectionState;


};
