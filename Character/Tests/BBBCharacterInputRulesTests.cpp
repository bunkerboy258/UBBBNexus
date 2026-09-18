#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterControlInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterEquipmentInput.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBCharacterInputRulesTest,
    "BBB.Character.Input.ConflictRules",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBCharacterInputRulesTest::RunTest(const FString &Parameters)
{
    // 穷举当前三个阻断条件 防止新增条件时误放行换弹或切换期间的开火
    for (int32 Flags = 0; Flags < 8; ++Flags)
    {
        const bool bReloading = (Flags & 1) != 0;
        const bool bSwitching = (Flags & 2) != 0;
        const bool bStartingReload = (Flags & 4) != 0;
        TestEqual(FString::Printf(TEXT("Fire rules %d"), Flags),
            FBBBCharacterFireInput::AllowsFire(bReloading, bSwitching, bStartingReload), Flags == 0);
    }
    TestFalse(TEXT("Aim exits sprint"), FBBBCharacterControlInput::AllowsSprint(true));
    TestTrue(TEXT("Released aim permits held sprint"), FBBBCharacterControlInput::AllowsSprint(false));
    return true;
}

#endif
