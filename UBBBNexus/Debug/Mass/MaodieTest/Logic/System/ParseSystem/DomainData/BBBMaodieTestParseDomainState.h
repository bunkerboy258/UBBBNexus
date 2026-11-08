#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/ParseSystem/DomainData/States/BBBMaodieTestParseState.h"
#include "BBBMaodieTestParseDomainState.generated.h"
/** 耄耋Parse领域 */
USTRUCT()
struct FBBBMaodieTestParseDomainState final
{
    GENERATED_BODY()
public:
    /** @return 当前领域只读状态 */
    const FBBBMaodieTestParseState &ReadMaodieTestParseState() const
    {
        return State;
    }
private:
    friend class FBBBMaodieTestParseProcessor;
    friend class FBBBMaodieTestInitializer;
    friend class FBBBMaodieTestShutdown;
    friend class FBBBMaodieTestParseProcessor;
    friend class FBBBMaodieTestParseSystem;
    UPROPERTY()
    FBBBMaodieTestParseState State;
};
