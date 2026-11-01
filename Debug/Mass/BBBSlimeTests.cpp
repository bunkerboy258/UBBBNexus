#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "MassEntitySubsystem.h"
#include "MassEntityConfigAsset.h"
#include "Misc/ScopeExit.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Processors/BBBSlimeProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeInputFragment.h"

/** 验证重叠最强效果 高度 遮挡 到期及实例版本边界 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBSlimeExposureTest, "UBBB.Mass.SpecialInfected.SlimeExposure",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBSlimeExposureTest::RunTest(const FString&)
{
    const auto Init = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Init);
    if (!TestNotNull(TEXT("粘液隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };
    auto* RuleA = NewObject<UBBBSlimeDefinition>(World);
    RuleA->EntityConfig = NewObject<UMassEntityConfigAsset>(World);
    auto* RuleB = NewObject<UBBBSlimeDefinition>(World);
    RuleB->EntityConfig = RuleA->EntityConfig;
    RuleB->DamagePerSecond = 35;
    RuleB->SpeedScale = 0.7f;
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Type = Manager.CreateArchetype({FBBBSlimeFragment::StaticStruct()});
    const auto A = Manager.CreateEntity(Type);
    const auto B = Manager.CreateEntity(Type);
    auto& FactA = Manager.GetFragmentDataChecked<FBBBSlimeFragment>(A).Fact;
    FactA.InstanceId = FGuid::NewGuid();
    FactA.SourceId = FGuid::NewGuid();
    FactA.Revision = 1;
    FactA.Definition = RuleA;
    FactA.Phase = 1;
    FactA.EndsAt = 8;
    auto& FactB = Manager.GetFragmentDataChecked<FBBBSlimeFragment>(B).Fact;
    FactB = FactA;
    FactB.InstanceId = FGuid::NewGuid();
    FactB.Definition = RuleB;
    FactB.Location = FVector(-20, 0, 0);
    float Damage = 0;
    float Scale = 1;
    TestTrue(TEXT("区域实际接触"), UBBBSlimeProcessor::QueryExposure(*World, FVector(0, 0, 5), 35, Damage, Scale));
    TestEqual(TEXT("重叠伤害取强 不累计"), Damage, 35.0f);
    TestEqual(TEXT("重叠减速取强 不相乘"), Scale, 0.5f);
    TestFalse(TEXT("楼上不受楼下区域影响"), UBBBSlimeProcessor::QueryExposure(*World, FVector(0, 0, 300), 35, Damage, Scale));
    TestFalse(TEXT("区域外无影响"), UBBBSlimeProcessor::QueryExposure(*World, FVector(600, 0, 5), 35, Damage, Scale));
    AActor* Wall = World->SpawnActor<AActor>();
    auto* Box = NewObject<UBoxComponent>(Wall);
    Wall->SetRootComponent(Box);
    Box->SetBoxExtent(FVector(5, 100, 100));
    Box->SetCollisionProfileName(TEXT("BlockAll"));
    Box->RegisterComponent();
    Wall->SetActorLocation(FVector(100, 0, 90));
    TestFalse(TEXT("墙后不受区域影响"), UBBBSlimeProcessor::QueryExposure(*World, FVector(200, 0, 5), 35, Damage, Scale));
    FactA.EndsAt = 0;
    FactB.EndsAt = 0;
    TestFalse(TEXT("到期即时解除"), UBBBSlimeProcessor::QueryExposure(*World, FVector(0, 0, 5), 35, Damage, Scale));
    TestEqual(TEXT("离开后恢复默认速度"), Scale, 1.0f);
    TestEqual(TEXT("离开后无持续伤害"), Damage, 0.0f);
    FBBBSlimeFragment Current;
    Current.Fact = FactA;
    FBBBSlimeStateAuthorityFactPacket Packet;
    Packet.Fact = FactA;
    TestFalse(TEXT("同版本不重复还原"), Packet.CanApply(Current));
    ++Packet.Fact.Revision;
    TestTrue(TEXT("新版本允许还原"), Packet.CanApply(Current));
    Packet.Fact.InstanceId = FGuid::NewGuid();
    TestFalse(TEXT("旧实例消息不能作用于另一实例"), Packet.CanApply(Current));
    return true;
}
#endif
