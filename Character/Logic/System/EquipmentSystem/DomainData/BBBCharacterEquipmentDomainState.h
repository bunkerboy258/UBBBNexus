#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentUseState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentUseInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentAnimationInputState.h"
#include "BBBCharacterEquipmentDomainState.generated.h"

class FBBBCharacterEquipmentSystem;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBCharacterEquipmentLifecycleProcessor;
class FBBBCharacterParseSystem;

/** 角色装备领域状态的唯一持有者 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentDomainState final
{
    GENERATED_BODY()

public:
    /** @return 当前装备独立使用状态 */
    const FBBBCharacterEquipmentUseState &ReadEquipmentUseState() const { return EquipmentUseState; }
    /** @return 待还原的当前装备使用结果 */
    const FBBBCharacterEquipmentUseInputState &ReadEquipmentUseInputState() const { return EquipmentUseInputState; }
    /** @return 角色装备选择状态 */
    const FBBBCharacterEquipmentSelectionState &ReadEquipmentSelectionState() const
    {
        return EquipmentSelectionState;
    }

    /** @return 等待转交的装备动画通知 */
    const FBBBCharacterEquipmentAnimationInputState &ReadEquipmentAnimationInputState() const
    {
        return EquipmentAnimationInputState;
    }

private:
    friend class FBBBCharacterEquipmentUseProcessor;
    /** 独立使用生命周期 */
    UPROPERTY(Transient)
    FBBBCharacterEquipmentUseState EquipmentUseState;
    /** 使用结果待解析数据 */
    FBBBCharacterEquipmentUseInputState EquipmentUseInputState;
    friend class FBBBCharacterEquipmentAnimationInputProcessor;
    /** 动画通知通信状态 */
    FBBBCharacterEquipmentAnimationInputState EquipmentAnimationInputState;

    friend class FBBBCharacterEquipmentSystem;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBCharacterEquipmentLifecycleProcessor;
    friend class FBBBCharacterParseSystem;

    /** 角色装备选择状态 */
    UPROPERTY()
    FBBBCharacterEquipmentSelectionState EquipmentSelectionState;


};
