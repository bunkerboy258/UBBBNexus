#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterControlInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBFireBehavior.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterInputRulesTest,
    "BBB.Character.Input.ConflictRules",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterInputRulesTest::RunTest(const FString &Parameters)
{
    TestTrue(TEXT("Fire without conflict"), FBBBFireBehavior::AllowsFire(false, false, false));
    TestFalse(TEXT("Reload blocks fire"), FBBBFireBehavior::AllowsFire(true, false, false));
    TestFalse(TEXT("Equip blocks fire"), FBBBFireBehavior::AllowsFire(false, true, false));
    TestFalse(TEXT("Starting reload blocks fire"), FBBBFireBehavior::AllowsFire(false, false, true));
    TestFalse(TEXT("Aim blocks sprint"), FBBBCharacterControlRule::AllowsSprint(true, false));
    TestFalse(TEXT("Fire blocks sprint"), FBBBCharacterControlRule::AllowsSprint(false, true));
    TestTrue(TEXT("Released aim permits sprint"), FBBBCharacterControlRule::AllowsSprint(false, false));
    return true;
}

#endif
