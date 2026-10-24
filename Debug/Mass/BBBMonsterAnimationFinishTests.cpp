#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "UObject/Script.h"
#include "UObject/UnrealType.h"
#include "UObject/Package.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterFactAnimInstance.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "Components/SkeletalMeshComponent.h"

/** 验证转身只消费实际朝向 跳变和战斗不能错误推进转身采样 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterAnimationFinishTest, "UBBB.Mass.ZombieAnimationFinish",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterAnimationFinishTest::RunTest(const FString& Parameters)
{
    FEditorScriptExecutionGuard ScriptGuard;
    auto* Source = LoadObject<UBBBMonsterPresentationDefinition>(nullptr,
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Connor/DA_BBBZombieConnorPresentation"));
    if (!TestNotNull(TEXT("独立表现配置"), Source))
    {
        return false;
    }
    auto* Rules = DuplicateObject<UBBBMonsterPresentationDefinition>(Source, GetTransientPackage());
    Rules->Animation.PursuitWalkCycleSeconds.X = 0;
    TestFalse(TEXT("零快走循环不得进入相位计算"), Rules->IsValid());
    Rules->Animation.PursuitWalkCycleSeconds.X = 0.766667;
    TestTrue(TEXT("有效快走循环恢复配置有效性"), Rules->IsValid());
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("动画表现测试世界"), World))
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
    for (const FString Gender : {FString(TEXT("Male")), FString(TEXT("Female"))})
    {
        const FString Package = TEXT("/Game/_Project/Mass/Monster/Zombie/") + Gender + TEXT("/BP_BBBZombie") + Gender + TEXT("Presentation");
        UClass* Class = LoadClass<ABBBMonsterPresentationActor>(nullptr, *(Package + TEXT(".BP_BBBZombie") + Gender + TEXT("Presentation_C")));
        if (!TestNotNull(TEXT("正式表现蓝图"), Class))
        {
            return false;
        }
        auto* Actor = World->SpawnActor<ABBBMonsterPresentationActor>(Class);
        auto* Instance = Actor ? Cast<UBBBMonsterFactAnimInstance>(Actor->GetMonsterMesh()->GetAnimInstance()) : nullptr;
        if (!TestNotNull(TEXT("正式事实动画实例"), Instance))
        {
            return false;
        }
        const auto* Phase = FindFProperty<FFloatProperty>(Instance->GetClass(), TEXT("FinishTurnPhase"));
        const auto* Weight = FindFProperty<FFloatProperty>(Instance->GetClass(), TEXT("FinishTurnWeight"));
        const auto* Yaw = FindFProperty<FFloatProperty>(Instance->GetClass(), TEXT("ActorYawFact"));
        if (!TestTrue(TEXT("正式图包含实际朝向与转身采样"), Phase && Weight && Yaw))
        {
            return false;
        }
        TestTrue(TEXT("实际朝向不能由蓝图写入"), Yaw->HasAllPropertyFlags(CPF_BlueprintReadOnly));
        auto* Presentation = Actor->GetMonsterPresentation();
        const auto Update = [Instance]()
        {
            Instance->NativeUpdateAnimation(1.0f / 60.0f);
            Instance->BlueprintThreadSafeUpdateAnimation(1.0f / 60.0f);
        };
        FBBBMonsterVariationFragment Variation;
        Variation.Seed = 123;
        Presentation->ApplyVariationFacts(Variation, 0.0f);
        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Idle, 0, 1, 1, 0);
        Actor->SetActorRotation(FRotator(0, 30, 0));
        Update();
        TestEqual(TEXT("出生朝向不应触发一次伪转身"), Phase->GetPropertyValue_InContainer(Instance), 0.0f);
        Actor->SetActorRotation(FRotator(0, 31.5f, 0));
        Update();
        TestEqual(TEXT("实际朝向原样提供给动画"), Yaw->GetPropertyValue_InContainer(Instance), 31.5f);
        TestTrue(TEXT("低速实际转向启动姿态混合"), Weight->GetPropertyValue_InContainer(Instance) > 0);
        const float TurnPhase = Phase->GetPropertyValue_InContainer(Instance);
        Update();
        TestEqual(TEXT("没有实际转向不能自行播放转身"), Phase->GetPropertyValue_InContainer(Instance), TurnPhase);
        Actor->SetActorRotation(FRotator(0, 175, 0));
        Update();
        TestEqual(TEXT("大角度重定位不能当作转身循环"), Phase->GetPropertyValue_InContainer(Instance), TurnPhase);
        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Attack, 0, 2, 2, 0.4f);
        Actor->SetActorRotation(FRotator(0, 176.5f, 0));
        Update();
        TestEqual(TEXT("攻击不推进站立转身"), Phase->GetPropertyValue_InContainer(Instance), TurnPhase);
        TestTrue(TEXT("动画不改写世界朝向"), FMath::IsNearlyEqual(Actor->GetActorRotation().Yaw, 176.5f));
        Variation.Seed = 456;
        Presentation->ApplyVariationFacts(Variation, 0.0f);
        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Idle, 0, 3, 1, 0);
        Actor->SetActorRotation(FRotator(0, 178, 0));
        Update();
        TestEqual(TEXT("载体复用不能继承上一代转身相位"), Phase->GetPropertyValue_InContainer(Instance), 0.0f);
        TestEqual(TEXT("载体复用不能继承上一代转身混合"), Weight->GetPropertyValue_InContainer(Instance), 0.0f);
        Actor->Destroy();
    }
    return true;
}

#endif
