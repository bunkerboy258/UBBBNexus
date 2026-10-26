#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterDestructionSystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterDismembermentSystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/AuthorityFact/Network/FBBBMonsterStateAuthorityFactPacket.h"
#include "Engine/SkeletalMesh.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDestructionMergeTest,
    "BBB.Monster.Gore.DestructionContributionConvergence", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDestructionMergeTest::RunTest(const FString&)
{
    FBBBMonsterDamageContribution First;
    First.PlayerId = 1;
    First.LastHitTime = 10;
    First.Parts.Torso = 20;
    for (int32 Index = 0; Index < 8; ++Index)
    {
        First.Destruction.Add(Index, 0.35f);
    }
    auto Later = First;
    Later.LastHitTime = 11;
    Later.Parts.Torso = 40;
    Later.Destruction.Add(0, 0.35f);
    auto Merged = First;
    Merged.Merge(Later);
    Merged.Merge(First);
    Merged.Merge(Later);
    TestTrue(TEXT("重复和迟到快照收敛到当前结果"), Merged == Later);
    auto Opposite = Later;
    Opposite.Merge(First);
    TestTrue(TEXT("合并顺序保持同一结果"), Opposite == Merged);

    FBBBMonsterDamageFragment Damage;
    Damage.Contributions.Add(1, Merged);
    FBBBMonsterDamageContribution Second;
    Second.PlayerId = 2;
    Second.Destruction.Add(0, 0.5f);
    Second.Destruction.Add(7, 0.5f);
    Damage.Contributions.Add(2, Second);
    FBBBMonsterGoreFragment State;
    FBBBMonsterDestructionSystem::Resolve(Damage, State);
    TestEqual(TEXT("多人共同命中封顶"), State.DestructionStrengths[0], static_cast<uint8>(255));
    TestTrue(TEXT("八个创口互相独立"), State.DestructionStrengths[7] > State.DestructionStrengths[6]);
    const auto Before = State;
    FBBBMonsterDestructionSystem::Resolve(Damage, State);
    TestTrue(TEXT("还原当前字典不会重复加深"), FMemory::Memcmp(Before.DestructionStrengths,
        State.DestructionStrengths, sizeof(State.DestructionStrengths)) == 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDestructionSelectionTest,
    "BBB.Monster.Gore.DestructionFrontBackSelection", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDestructionSelectionTest::RunTest(const FString&)
{
    auto* Definition = NewObject<UBBBMonsterGoreDefinition>();
    Definition->Body = NewObject<USkeletalMesh>();
    for (int32 Index = 0; Index < 8; ++Index)
    {
        auto& Region = Definition->Destruction->Regions.AddDefaulted_GetRef();
        const bool bFront = Index < 4;
        const float X = Index % 2 == 0 ? 10 : -10;
        const float Z = Index % 4 < 2 ? 130 : 105;
        Region.Contact = FVector(X, bFront ? 10 : -10, Z);
        Region.Rotation = FRotationMatrix::MakeFromXZ(bFront ? -FVector::ForwardVector : FVector::ForwardVector,
            bFront ? FVector::RightVector : -FVector::RightVector).Rotator();
        Region.Extent = FVector(26, 28, 12);
    }
    for (int32 Index = 0; Index < 8; ++Index)
    {
        const auto& Region = Definition->Destruction->Regions[Index];
        const FVector Normal = Region.Rotation.RotateVector(FVector::UpVector);
        TestEqual(TEXT("正背面与上下区按真实入射方向匹配"), FBBBMonsterDestructionSystem::Select(*Definition,
            Region.Contact + Normal * 5, -Normal, FTransform::Identity, nullptr), Index);
    }
    TestEqual(TEXT("无效方向拒绝选区"), FBBBMonsterDestructionSystem::Select(*Definition,
        FVector::ZeroVector, FVector::ZeroVector, FTransform::Identity, nullptr), INDEX_NONE);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterGoreResultRestoreTest,
    "BBB.Monster.Gore.CurrentResultRestore", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterGoreResultRestoreTest::RunTest(const FString&)
{
    auto* Definition = NewObject<UBBBMonsterGoreDefinition>();
    for (int32 Region = 1; Region < 6; ++Region)
    {
        auto& Part = Definition->Dismemberment->Parts.AddDefaulted_GetRef();
        Part.Id = Region + 10;
        Part.Region = static_cast<EBBBMonsterHitRegion>(Region);
    }
    FBBBMonsterGoreFragment State;
    FBBBMonsterHitReactionFragment Hit;
    FBBBMonsterDismembermentSystem::Resolve(State, *Definition, 0x3e, Hit, nullptr);
    FBBBMonsterDismembermentSystem::Resolve(State, *Definition, 0, Hit, nullptr);
    for (int32 Index = 0; Index < 5; ++Index)
    {
        TestEqual(TEXT("五个断点持续保留"), State.DismembermentPresets[Index], static_cast<uint8>(Index + 11));
    }
    FBBBMonsterStateAuthorityFactPacket Packet;
    Packet.InstanceId = FGuid::NewGuid();
    Packet.Revision = 2;
    FMemory::Memcpy(Packet.DismembermentPresets, State.DismembermentPresets, sizeof(Packet.DismembermentPresets));
    FBBBMonsterNetworkFragment Network;
    FTransformFragment Position;
    FMassVelocityFragment Velocity;
    FBBBMonsterBehaviorFragment Behavior;
    FBBBMonsterGoreFragment Remote;
    Remote.DestructionStrengths[7] = 200;
    Packet.Apply(Network, Position, Velocity, Behavior, Remote);
    TestTrue(TEXT("五个断点直接还原"), FMemory::Memcmp(State.DismembermentPresets,
        Remote.DismembermentPresets, sizeof(State.DismembermentPresets)) == 0);
    TestEqual(TEXT("行动快照保持独立毁伤贡献结果"), Remote.DestructionStrengths[7], static_cast<uint8>(200));
    TestFalse(TEXT("重复版本被入口拒绝"), Packet.CanApply(Network));
    Packet.Revision = 1;
    TestFalse(TEXT("迟到版本被入口拒绝"), Packet.CanApply(Network));
    Packet.InstanceId = FGuid::NewGuid();
    Packet.Revision = 3;
    TestFalse(TEXT("跨代结果被入口拒绝"), Packet.CanApply(Network));
    Remote = FBBBMonsterGoreFragment();
    for (const auto Value : Remote.DestructionStrengths)
    {
        TestEqual(TEXT("复用后八区清零"), Value, static_cast<uint8>(0));
    }
    for (const auto Value : Remote.DismembermentPresets)
    {
        TestEqual(TEXT("复用后五个断点清零"), Value, static_cast<uint8>(0));
    }
    return true;
}
#endif
