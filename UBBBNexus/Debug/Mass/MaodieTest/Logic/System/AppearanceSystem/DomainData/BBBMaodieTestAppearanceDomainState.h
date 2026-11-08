#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AppearanceSystem/DomainData/States/BBBMaodieTestAppearanceState.h"
#include "BBBMaodieTestAppearanceDomainState.generated.h"

/** 耄耋外观领域的唯一状态持有者 */
USTRUCT()
struct FBBBMaodieTestAppearanceDomainState final
{
    GENERATED_BODY()
public:
    /** @return 本机外观只读状态 */
    const FBBBMaodieTestAppearanceState &ReadMaodieTestAppearanceState() const
    {
        return State;
    }

private:
    friend class FBBBMaodieTestAppearanceProcessor;

    UPROPERTY()
    FBBBMaodieTestAppearanceState State;
};
