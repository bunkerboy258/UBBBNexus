#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/World.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterSteering.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

/** 朝向模式在攻击距离边界和短时停走扰动下保持稳定 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterSteeringFacingTest, "UBBB.Mass.ZombieSteering.Facing",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterSteeringFacingTest::RunTest(const FString& Parameters)
{
    FBBBMonsterMovementFragment Movement;
    FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, false, 109.0f, 110.0f, 45.0f, 0.0f);
    TestTrue(TEXT("进入近战接近距离后面向玩家"), Movement.bFacingTarget);
    for (int32 Frame = 1; Frame < 300; ++Frame)
    {
        const float Distance = Frame % 2 == 0 ? 109.0f : 111.0f;
        FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, Frame % 2 == 0,
            Distance, 110.0f, 45.0f, Frame / 60.0f);
        TestTrue(TEXT("距离边界和单帧停走不反复切换朝向依据"), Movement.bFacingTarget);
    }
    FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, false, 200.0f, 110.0f, 45.0f, 6.0f);
    TestFalse(TEXT("离开退出缓冲后恢复移动朝向"), Movement.bFacingTarget);
    for (int32 Frame = 0; Frame < 180; ++Frame)
    {
        FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, Frame % 12 < 6,
            400.0f, 110.0f, 45.0f, 7.0f + Frame / 60.0f);
        TestFalse(TEXT("不足四分之一秒的拥挤停走保持行走朝向"), Movement.bFacingTarget);
    }
    FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, true, 400.0f, 110.0f, 45.0f, 11.0f);
    FBBBMonsterSteering::UpdateFacingMode(Movement, true, false, true, true, 400.0f, 110.0f, 45.0f, 11.3f);
    TestTrue(TEXT("持续拥挤等待后面向玩家"), Movement.bFacingTarget);
    FBBBMonsterSteering::UpdateFacingMode(Movement, false, false, false, false, 0.0f, 110.0f, 45.0f, 12.0f);
    TestFalse(TEXT("丢失目标清除目标朝向模式"), Movement.bFacingTarget);
    return true;
}

#endif
