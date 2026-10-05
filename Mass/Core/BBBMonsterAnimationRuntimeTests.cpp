#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"

#include "Animation/AnimSequenceBase.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterAnimInstance.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

/** 验证仍使用原双通道基类的既有测试体 僵尸改由事实测试覆盖 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterAnimationRuntimeTest, "UBBB.Mass.AnimationSnapshots",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterAnimationRuntimeTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false)
        .CreatePhysicsScene(false)
        .CreateNavigation(false)
        .CreateAISystem(false)
        .ShouldSimulatePhysics(false);
    UWorld* const World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("动画测试世界"), World))
    {
        return false;
    }

    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);

    ON_SCOPE_EXIT
    {
        World->DestroyWorld(false);
        GEngine->DestroyWorldContext(World);
    };

    const TArray<FString> Classes =
    {
        TEXT("/Game/_Project/System/Mass/Monster/BP_BBBMonster.BP_BBBMonster_C")
    };

    for (const FString& ClassPath : Classes)
    {
        UClass* const ActorClass = LoadClass<ABBBMonsterPresentationActor>(nullptr, *ClassPath);
        if (!TestNotNull(ClassPath, ActorClass))
        {
            return false;
        }

        ABBBMonsterPresentationActor* const Actor = World->SpawnActor<ABBBMonsterPresentationActor>(ActorClass);
        if (!TestNotNull(TEXT("测试表现演员"), Actor))
        {
            return false;
        }

        UBBBMonsterAnimInstance* const Animation = Cast<UBBBMonsterAnimInstance>(Actor->GetMonsterMesh()->GetAnimInstance());
        if (!TestNotNull(TEXT("全部表现演员使用统一动画实例"), Animation))
        {
            return false;
        }

        UBBBMonsterPresentationComponent* const Presentation = Actor->GetMonsterPresentation();
        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Idle, 0.0f, 0.0f, 0, 0.0f);
        Animation->NativeUpdateAnimation(0.05f);
        TestNotNull(TEXT("待机动画已采样"), Animation->GetActiveAnimation());
        const bool bIdleChannel = Animation->IsChannelBActive();
        USkeletalMeshComponent* const Mesh = Actor->GetMonsterMesh();
        Mesh->TickAnimation(0.0f, false);
        Mesh->RefreshBoneTransforms();
        const FVector IdleHand = Mesh->GetSocketTransform(TEXT("hand_l"), RTS_Component).GetLocation();

        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Attack, 0.0f, 1.0f, 1, 0.4f);
        Animation->NativeUpdateAnimation(0.05f);
        TestTrue(TEXT("新动作切换通道保留旧姿势"), bIdleChannel != Animation->IsChannelBActive());
        UAnimSequenceBase* const Attack = Animation->GetActiveAnimation();
        if (!TestNotNull(TEXT("单次攻击动画"), Attack))
        {
            return false;
        }

        TestTrue(TEXT("攻击时间由 Mass 进度定位"), FMath::IsNearlyEqual(Animation->GetActiveAnimationTime(), Attack->GetPlayLength() * 0.4f, 0.001f));
        Mesh->TickAnimation(0.25f, false);
        Mesh->RefreshBoneTransforms();
        if (Mesh->GetBoneIndex(TEXT("hand_l")) != INDEX_NONE)
        {
            const FVector AttackHand = Mesh->GetSocketTransform(TEXT("hand_l"), RTS_Component).GetLocation();
            TestTrue(TEXT("实际动画图必须产生不同于待机的攻击手部姿势"), FVector::Dist(IdleHand, AttackHand) > 1.0f);
        }

        const bool bAttackChannel = Animation->IsChannelBActive();
        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Attack, 0.0f, 1.0f, 1, 0.6f);
        Animation->NativeUpdateAnimation(0.2f);
        TestEqual(TEXT("同一动作更新不得切换通道"), Animation->IsChannelBActive(), bAttackChannel);
        TestTrue(TEXT("攻击采样不依赖动画帧时间"), FMath::IsNearlyEqual(Animation->GetActiveAnimationTime(), Attack->GetPlayLength() * 0.6f, 0.001f));

        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Attack, 0.0f, 2.0f, 2, 0.0f);
        Animation->NativeUpdateAnimation(0.05f);
        TestTrue(TEXT("同状态新编号也重新过渡"), Animation->IsChannelBActive() != bAttackChannel);
        TestEqual(TEXT("新攻击从逻辑零进度开始"), Animation->GetActiveAnimationTime(), 0.0f);

        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Hurt, 0.0f, 3.0f, 3, 0.5f);
        Animation->NativeUpdateAnimation(0.05f);
        TestTrue(TEXT("受伤打断仍服从逻辑进度"), FMath::IsNearlyEqual(Animation->GetActiveAnimationTime(), Animation->GetActiveAnimation()->GetPlayLength() * 0.5f, 0.001f));

        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Dead, 0.0f, 4.0f, 4, 1.0f);
        Animation->NativeUpdateAnimation(0.05f);
        const float DeathEnd = Animation->GetActiveAnimation()->GetPlayLength();
        TestEqual(TEXT("死亡停在末帧"), Animation->GetActiveAnimationTime(), DeathEnd);
        Animation->NativeUpdateAnimation(1.0f);
        TestEqual(TEXT("死亡不会自行循环或推进"), Animation->GetActiveAnimationTime(), DeathEnd);
        TestTrue(TEXT("演员只读表现不移动世界位置"), Actor->GetActorLocation().Equals(FVector::ZeroVector));
        Actor->Destroy();
    }

    return true;
}

#endif
