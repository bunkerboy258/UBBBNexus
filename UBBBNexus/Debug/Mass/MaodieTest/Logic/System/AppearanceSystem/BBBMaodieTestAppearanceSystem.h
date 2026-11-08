#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AppearanceSystem/Processors/BBBMaodieTestAppearanceProcessor.h"

/** 耄耋外观调度根 */
class FBBBMaodieTestAppearanceSystem final
{
public:
    /** @param Actor 耄耋 @param DeltaSeconds 帧间隔 @return 无 */
    static void Update(ABBBMaodieTest &Actor, float DeltaSeconds)
    {
        FBBBMaodieTestAppearanceProcessor::Update(Actor, DeltaSeconds);
    }

    /** @param Actor 耄耋 @return 无 */
    static void Shutdown(ABBBMaodieTest &Actor)
    {
        FBBBMaodieTestAppearanceProcessor::Shutdown(Actor);
    }
};
