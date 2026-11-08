#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/BBBMaodieTestBehaviorDomainState.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AnimationSystem/DomainData/BBBMaodieTestAnimationDomainState.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/ParseSystem/DomainData/BBBMaodieTestParseDomainState.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/NetworkSystem/DomainData/BBBMaodieTestNetworkDomainState.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AppearanceSystem/DomainData/BBBMaodieTestAppearanceDomainState.h"
#include "BBBMaodieTestRuntimeData.generated.h"
/** 耄耋唯一运行时黑板 */
USTRUCT()
struct FBBBMaodieTestRuntimeData final
{
    GENERATED_BODY()

    /** 本机外观领域 */
    UPROPERTY()
    FBBBMaodieTestAppearanceDomainState Appearance;
    /** Behavior领域 */
    UPROPERTY()
    FBBBMaodieTestBehaviorDomainState Behavior;
    /** Animation领域 */
    UPROPERTY()
    FBBBMaodieTestAnimationDomainState Animation;
    /** Parse领域 */
    UPROPERTY()
    FBBBMaodieTestParseDomainState Parse;
    /** Network领域 */
    UPROPERTY()
    FBBBMaodieTestNetworkDomainState Network;
};
