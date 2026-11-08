#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/NetworkSystem/DomainData/States/BBBMaodieTestNetworkState.h"
#include "BBBMaodieTestNetworkDomainState.generated.h"
/** 耄耋Network领域 */
USTRUCT()
struct FBBBMaodieTestNetworkDomainState final
{
    GENERATED_BODY()
public:
    /** @return 当前领域只读状态 */
    const FBBBMaodieTestNetworkState &ReadMaodieTestNetworkState() const
    {
        return State;
    }
private:
    friend class FBBBMaodieTestNetworkProcessor;
    friend class FBBBMaodieTestInitializer;
    friend class FBBBMaodieTestShutdown;
    friend class FBBBMaodieTestParseProcessor;
    friend class FBBBMaodieTestParseSystem;
    UPROPERTY()
    FBBBMaodieTestNetworkState State;
};
