#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "BBBMonsterDirectorScheduleProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorInputFragment.h"

/** 验证 L4D 三通道导演预算计算与核心控制输入包 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDirectorBudgetTest, "UBBB.Mass.MonsterDirector.BudgetAndMessages",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDirectorBudgetTest::RunTest(const FString& Parameters)
{
    FBBBMonsterDirectorEventWaveDefinition Wave;
    Wave.EventId = TEXT("Alarm01");
    Wave.Mode = EBBBMonsterDirectorEventWaveMode::Burst;
    Wave.TotalCount = 12;
    Wave.BatchCount = 5;
    Wave.MaxAlive = 10;

    TestEqual(TEXT("正常批次受 BatchCount 限制"), UBBBMonsterDirectorScheduleProcessor::CalculateEventRequest(Wave, 0, 0, 20), 5);
    TestEqual(TEXT("受事件存活上限裁剪"), UBBBMonsterDirectorScheduleProcessor::CalculateEventRequest(Wave, 0, 8, 20), 2);
    TestEqual(TEXT("受世界存活上限裁剪"), UBBBMonsterDirectorScheduleProcessor::CalculateEventRequest(Wave, 0, 0, 3), 3);
    TestEqual(TEXT("剩余总量不足时仅取剩余"), UBBBMonsterDirectorScheduleProcessor::CalculateEventRequest(Wave, 10, 0, 10), 2);
    TestEqual(TEXT("达到总量上限后停止申请"), UBBBMonsterDirectorScheduleProcessor::CalculateEventRequest(Wave, 12, 0, 10), 0);

    Wave.Mode = EBBBMonsterDirectorEventWaveMode::Sustained;
    Wave.TotalCount = 0;
    TestEqual(TEXT("持续补员无限上限时持续按批次补齐"), UBBBMonsterDirectorScheduleProcessor::CalculateEventRequest(Wave, 100, 6, 10), 4);

    auto* Definition = NewObject<UBBBMonsterDirectorDefinition>();
    Definition->EventWaves.Add(Wave);

    FBBBMonsterDirectorFragment State;
    State.InstanceId = FGuid::NewGuid();
    State.Definition = Definition;
    FBBBMonsterDirectorSpawnFragment Spawn;

    FBBBMonsterDirectorStartLocalControlPacket Start;
    TestTrue(TEXT("未运行状态允许启动"), Start.CanApply(State, Spawn));
    Start.Apply(State, Spawn);
    TestTrue(TEXT("启动后进入运行状态"), State.bRunning && State.Generation == 1);

    FBBBMonsterDirectorSetDynamicHordePausedLocalControlPacket PauseHorde;
    PauseHorde.bPaused = true;
    PauseHorde.Apply(State, Spawn);
    TestTrue(TEXT("允许暂停动态波次"), State.bDynamicHordePaused);

    FBBBMonsterDirectorTriggerEventWaveLocalControlPacket Trigger;
    Trigger.EventId = TEXT("Alarm01");
    TestTrue(TEXT("配置存在的事件允许触发"), Trigger.IsValid() && Trigger.CanApply(State, Spawn));
    Trigger.Apply(State, Spawn);
    TestEqual(TEXT("触发后进入活跃事件表"), State.ActiveEvents.Num(), 1);

    FBBBMonsterDirectorStopEventWaveLocalControlPacket StopEvent;
    StopEvent.EventId = TEXT("Alarm01");
    StopEvent.Apply(State, Spawn);
    TestEqual(TEXT("停止指定事件后清空"), State.ActiveEvents.Num(), 0);

    FBBBMonsterDirectorStopLocalControlPacket Stop;
    TestTrue(TEXT("运行中允许停止"), Stop.CanApply(State, Spawn));
    Stop.Apply(State, Spawn);
    TestFalse(TEXT("停止后退出运行状态"), State.bRunning);
    return true;
}

#endif
