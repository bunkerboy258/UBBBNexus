#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "MassEntityConfigAsset.h"

/** 验证同一只僵尸的三档速度 加减速与停靠边界 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterLocomotionTest, "UBBB.Mass.ZombieLocomotion",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterLocomotionTest::RunTest(const FString& Parameters)
{
    FBBBMonsterMovementFragment Movement;
    TestEqual(TEXT("巡逻始终步行"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 2000.0f, true), EBBBMonsterGait::Walk);
    TestEqual(TEXT("近距离追击仍然跑步"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 150.0f, false), EBBBMonsterGait::Run);
    TestEqual(TEXT("刚进入追击不使用步行"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 250.0f, false), EBBBMonsterGait::Run);
    TestEqual(TEXT("中距离跑步"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 500.0f, false), EBBBMonsterGait::Run);
    TestEqual(TEXT("远距离冲刺"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 1500.0f, false), EBBBMonsterGait::Sprint);
    Movement.Gait = EBBBMonsterGait::Sprint;
    TestEqual(TEXT("冲刺接近后不降档"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 700.0f, false), EBBBMonsterGait::Sprint);
    TestEqual(TEXT("冲刺贴近目标仍不降档"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 1.0f, false), EBBBMonsterGait::Sprint);
    TestEqual(TEXT("冲刺后恢复巡逻仍然步行"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 1500.0f, true), EBBBMonsterGait::Walk);
    Movement.Gait = EBBBMonsterGait::Run;
    TestEqual(TEXT("跑步贴近目标不降档"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 1.0f, false), EBBBMonsterGait::Run);
    float Speed = 0.0f;
    const float Targets[] = { Movement.WalkSpeed, Movement.RunSpeed, Movement.SprintSpeed, Movement.RunSpeed, Movement.WalkSpeed };
    for (const float Target : Targets)
    {
        for (int32 Frame = 0; Frame < 120; ++Frame)
        {
            const float Previous = Speed;
            Speed = UBBBMonsterLocomotionProcessor::CalculateSpeed(Movement, Speed, Target, 2000.0f, 1.0f / 60.0f);
            TestTrue(TEXT("每帧变化遵守加减速"), FMath::Abs(Speed - Previous) <= Movement.Deceleration / 60.0f + KINDA_SMALL_NUMBER);
        }
        TestEqual(TEXT("同一实例到达目标速度"), Speed, Target);
    }
    TestEqual(TEXT("到达立即停止"), UBBBMonsterLocomotionProcessor::CalculateSpeed(Movement, 500.0f, 500.0f, 0.0f, 0.016f), 0.0f);
    TestEqual(TEXT("零时间没有虚构速度"), UBBBMonsterLocomotionProcessor::CalculateSpeed(Movement, 500.0f, 500.0f, 1000.0f, 0.0f), 0.0f);
    TestEqual(TEXT("近终点不提前制动"), UBBBMonsterLocomotionProcessor::CalculateSpeed(Movement, 500.0f, 500.0f, 1.0f, 0.016f), 500.0f);
    TestEqual(TEXT("近终点保持跑速"), UBBBMonsterLocomotionProcessor::CalculateSpeed(Movement, 300.0f, 300.0f, 1.0f, 0.016f), 300.0f);
    UBBBMonsterDefinition* Definition = NewObject<UBBBMonsterDefinition>();
    Definition->EntityConfig = NewObject<UMassEntityConfigAsset>();
    TestTrue(TEXT("新配置默认有效"), Definition->IsValid());
    Definition->RunSpeed = Definition->WalkSpeed;
    TestFalse(TEXT("错乱档位必须拒绝"), Definition->IsValid());
    return true;
}

#endif
