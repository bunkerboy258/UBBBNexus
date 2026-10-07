#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/States/BBBCharacterPhysicalPresentationState.h"
#include "BBBCharacterPhysicalPresentationDomainState.generated.h"

/** 角色物理表现状态的唯一持有者 */
USTRUCT()
struct FBBBCharacterPhysicalPresentationDomainState final
{
    GENERATED_BODY()
  public:
    /** @return 已应用的物理表现状态 */
    const FBBBCharacterPhysicalPresentationState &ReadPhysicalPresentationState() const
    {
        return PhysicalPresentationState;
    }

  private:
    friend class FBBBCharacterPhysicalPresentationProcessor;
    /** 独立物理表现应用记录 */
    UPROPERTY(Transient)
    FBBBCharacterPhysicalPresentationState PhysicalPresentationState;
};
