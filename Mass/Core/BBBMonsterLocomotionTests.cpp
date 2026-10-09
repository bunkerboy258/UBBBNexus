#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"
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

    const FQuat InitialFacing = FRotator(0.0f, 45.0f, 0.0f).Quaternion();
    const float FrameSeconds = 1.0f / 60.0f;
    TestEqual(TEXT("无主动移动的碰撞修正不转身"), UBBBMonsterLocomotionProcessor::CalculateFacing(
        InitialFacing, FVector::ZeroVector, FVector(-100.0f, 100.0f, 0.0f), FrameSeconds), InitialFacing);
    TestEqual(TEXT("低速接触位移保持朝向"), UBBBMonsterLocomotionProcessor::CalculateFacing(
        InitialFacing, FVector(1.0f, 0.0f, 0.0f), FVector(0.0f, 9.0f, -200.0f), FrameSeconds), InitialFacing);
    TestEqual(TEXT("反向碰撞修正不触发掉头"), UBBBMonsterLocomotionProcessor::CalculateFacing(
        InitialFacing, FVector(1.0f, 0.0f, 0.0f), FVector(-100.0f, 0.0f, 0.0f), FrameSeconds), InitialFacing);
    TestEqual(TEXT("零步长不改变朝向"), UBBBMonsterLocomotionProcessor::CalculateFacing(
        InitialFacing, FVector(1.0f, 0.0f, 0.0f), FVector(100.0f, 0.0f, 0.0f), 0.0f), InitialFacing);
    TestEqual(TEXT("碰撞侧滑不带偏主动移动朝向"), UBBBMonsterLocomotionProcessor::CalculateFacing(
        FQuat::Identity, FVector(5.0f, 0.0f, 0.0f), FVector(12.0f, 100.0f, 0.0f), FrameSeconds), FQuat::Identity);
    const FQuat Turn = UBBBMonsterLocomotionProcessor::CalculateFacing(FQuat::Identity,
        FVector(0.0f, 1.0f, 0.0f), FVector(0.0f, 300.0f, 0.0f), FrameSeconds);
    TestTrue(TEXT("正常转身限制为每帧六度"), FMath::IsNearlyEqual(Turn.Rotator().Yaw, 6.0f, 0.001f));
    const FQuat Wrapped = UBBBMonsterLocomotionProcessor::CalculateFacing(FRotator(0.0f, 179.0f, 0.0f).Quaternion(),
        FRotator(0.0f, -179.0f, 0.0f).Vector(), FRotator(0.0f, -179.0f, 0.0f).Vector() * 300.0f, FrameSeconds);
    TestTrue(TEXT("跨越正负一百八十度选择最短转向"), FMath::IsNearlyEqual(Wrapped.Rotator().Yaw, -179.0f, 0.001f));
    for (const float Rate : {30.0f, 60.0f, 120.0f})
    {
        FQuat Facing = FQuat::Identity;
        float Elapsed = 0.0f;
        while (Elapsed < 0.25f)
        {
            const float StepSeconds = FMath::Min(1.0f / Rate, 0.25f - Elapsed);
            Facing = UBBBMonsterLocomotionProcessor::CalculateFacing(Facing,
                FVector(0.0f, 1.0f, 0.0f), FVector(0.0f, 300.0f, 0.0f), StepSeconds);
            Elapsed += StepSeconds;
        }
        TestTrue(TEXT("不同帧率四分之一秒完成九十度转身"), FMath::IsNearlyEqual(Facing.Rotator().Yaw, 90.0f, 0.001f));
    }

    UBBBMonsterDefinition* Definition = NewObject<UBBBMonsterDefinition>();
    Definition->EntityConfig = NewObject<UMassEntityConfigAsset>();
    Definition->Variation = NewObject<UBBBMonsterVariationDefinition>();
    TestTrue(TEXT("新配置默认有效"), Definition->IsValid());
    Definition->RunSpeed = Definition->WalkSpeed;
    TestFalse(TEXT("错乱档位必须拒绝"), Definition->IsValid());
    return true;
}

#endif
