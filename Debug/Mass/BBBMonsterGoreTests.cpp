#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGorePresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/AuthorityFact/Network/FBBBMonsterStateAuthorityFactPacket.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterUnifiedWoundLifetimeTest,
    "BBB.Monster.Gore.UnifiedWoundLifetime", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterUnifiedWoundLifetimeTest::RunTest(const FString&)
{
    auto* Definition = NewObject<UBBBMonsterGoreDefinition>();
    for (int32 Region = 1; Region < 6; ++Region)
    {
        auto& Preset = Definition->Wounds.AddDefaulted_GetRef();
        Preset.Id = Region + 10;
        Preset.Region = static_cast<EBBBMonsterHitRegion>(Region);
        Preset.bPermanent = true;
    }
    FBBBMonsterGoreFragment State;
    FBBBMonsterHitReactionFragment Hit;
    UBBBMonsterGorePresentationComponent::UpdateWounds(State, *Definition, 0x3e, Hit, nullptr, 1, true);
    for (int32 Index = 0; Index < 5; ++Index)
    {
        TestEqual(TEXT("无显示载体也可选择永久伤口"), State.Wounds[Index].PresetId, static_cast<uint8>(Index + 11));
        TestEqual(TEXT("永久伤口权重"), State.Wounds[Index].Strength, 1.0f);
    }
    TestEqual(TEXT("普通伤口预算仍为空"), State.Wounds[5].PresetId, static_cast<uint8>(0));
    TestEqual(TEXT("总预算固定七处"), static_cast<int32>(UE_ARRAY_COUNT(State.Wounds)), 7);

    UBBBMonsterGorePresentationComponent::UpdateWounds(State, *Definition, 0, Hit, nullptr, 2, true);
    TestEqual(TEXT("后续空命中不恢复已断开的肢体"), State.Wounds[1].PresetId, static_cast<uint8>(12));

    FBBBMonsterGoreFragment Remote;
    UBBBMonsterGorePresentationComponent::UpdateWounds(Remote, *Definition, 0x3e, Hit, nullptr, 1, false);
    TestEqual(TEXT("镜像不自行选择永久外观"), Remote.Wounds[0].PresetId, static_cast<uint8>(0));

    State = FBBBMonsterGoreFragment();
    for (const auto& Wound : State.Wounds)
    {
        TestEqual(TEXT("新一代清除全部表现"), Wound.PresetId, static_cast<uint8>(0));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterUnifiedWoundRestoreTest,
    "BBB.Monster.Gore.CurrentResultRestore", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterUnifiedWoundRestoreTest::RunTest(const FString&)
{
    FBBBMonsterStateAuthorityFactPacket Packet;
    Packet.InstanceId = FGuid::NewGuid();
    Packet.Revision = 2;
    const uint8 Presets[] = {10, 21, 30, 41, 50};
    FMemory::Memcpy(Packet.WoundPresets, Presets, sizeof(Presets));
    FBBBMonsterNetworkFragment Network;
    FTransformFragment Position;
    FMassVelocityFragment Velocity;
    FBBBMonsterBehaviorFragment Behavior;
    FBBBMonsterGoreFragment Gore;
    Gore.Wounds[5].PresetId = 2;
    Gore.Wounds[6].PresetId = 3;
    TestTrue(TEXT("当前完整外观结果可还原"), Packet.IsValid() && Packet.CanApply(Network));
    Packet.Apply(Network, Position, Velocity, Behavior, Gore);
    for (int32 Index = 0; Index < 5; ++Index)
    {
        TestEqual(TEXT("五处永久伤口身份不依赖命中重播"), Gore.Wounds[Index].PresetId, Presets[Index]);
        TestEqual(TEXT("还原永久权重"), Gore.Wounds[Index].Strength, 1.0f);
    }
    TestEqual(TEXT("本地短时创口不被远端覆盖"), Gore.Wounds[5].PresetId, static_cast<uint8>(2));
    TestEqual(TEXT("第二处本地创口不被远端覆盖"), Gore.Wounds[6].PresetId, static_cast<uint8>(3));
    TestFalse(TEXT("重复版本不重复应用"), Packet.CanApply(Network));
    Packet.Revision = 1;
    TestFalse(TEXT("旧版本不得回退外观"), Packet.CanApply(Network));
    Packet.Revision = 3;
    Packet.InstanceId = FGuid::NewGuid();
    TestFalse(TEXT("旧实例消息不得进入新实例"), Packet.CanApply(Network));
    Packet.InstanceId = Network.InstanceId;
    Behavior.State = EBBBMonsterBehavior::Dead;
    Packet.Behavior = EBBBMonsterBehavior::Chase;
    Packet.Apply(Network, Position, Velocity, Behavior, Gore);
    TestEqual(TEXT("行动消息不能复活本地死亡"), Behavior.State, EBBBMonsterBehavior::Dead);
    return true;
}
#endif
