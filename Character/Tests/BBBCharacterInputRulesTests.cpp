#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBControlBehaviors.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBFireBehavior.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBEquipBehavior.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterInputRulesTest,
    "BBB.Character.Input.ConflictRules",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterInputRulesTest::RunTest(const FString &Parameters)
{
    TestFalse(TEXT("Fire without conflict"), FBBBFireBehavior::Policy.IsBlocked(0));
    TestTrue(TEXT("Reload blocks fire"),
        FBBBFireBehavior::Policy.IsBlocked(BBBBehaviorGroup::Reload));
    TestTrue(TEXT("Equip blocks fire"),
        FBBBFireBehavior::Policy.IsBlocked(BBBBehaviorGroup::Equip));
    TestTrue(TEXT("Equip cancels reload"),
        (FBBBEquipBehavior::Policy.Cancels & BBBBehaviorGroup::Reload) != 0);
    TestTrue(TEXT("Aim blocks sprint"),
        FBBBSprintBehavior::Policy.IsBlocked(BBBBehaviorGroup::Aim));
    TestTrue(TEXT("Fire blocks sprint"),
        FBBBSprintBehavior::Policy.IsBlocked(BBBBehaviorGroup::Fire));
    TestFalse(TEXT("Released aim permits sprint"),
        FBBBSprintBehavior::Policy.IsBlocked(0));
    return true;
}

#endif
