#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AnimationSystem/DomainData/States/BBBMaodieTestAnimationState.h"
#include "BBBMaodieTestAnimationDomainState.generated.h"
/** 耄耋Animation领域 */
USTRUCT()
struct FBBBMaodieTestAnimationDomainState final
{
    GENERATED_BODY()
public:
    /** @return 当前领域只读状态 */
    const FBBBMaodieTestAnimationState &ReadMaodieTestAnimationState() const
    {
        return State;
    }
private:
    friend class FBBBMaodieTestAnimationProcessor;
    friend class FBBBMaodieTestInitializer;
    friend class FBBBMaodieTestShutdown;
    friend class FBBBMaodieTestParseProcessor;
    friend class FBBBMaodieTestParseSystem;
    UPROPERTY()
    FBBBMaodieTestAnimationState State;
};
