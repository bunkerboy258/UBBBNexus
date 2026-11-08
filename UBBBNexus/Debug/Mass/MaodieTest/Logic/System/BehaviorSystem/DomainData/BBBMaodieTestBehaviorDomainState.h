#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/States/BBBMaodieTestBehaviorState.h"
#include "BBBMaodieTestBehaviorDomainState.generated.h"
/** 耄耋Behavior领域 */
USTRUCT()
struct FBBBMaodieTestBehaviorDomainState final
{
    GENERATED_BODY()
public:
    /** @return 当前领域只读状态 */
    const FBBBMaodieTestBehaviorState &ReadMaodieTestBehaviorState() const
    {
        return State;
    }
private:
    friend class FBBBMaodieTestBehaviorProcessor;
    friend class FBBBMaodieTestInitializer;
    friend class FBBBMaodieTestShutdown;
    friend class FBBBMaodieTestParseProcessor;
    friend class FBBBMaodieTestParseSystem;
    UPROPERTY()
    FBBBMaodieTestBehaviorState State;
};
