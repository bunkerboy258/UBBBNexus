#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "UObject/Script.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"

namespace
{
    class FBBBMonsterCorpseHeldCommand final : public IAutomationLatentCommand
    {
    public:
        FBBBMonsterCorpseHeldCommand(FAutomationTestBase* InTest, UWorld* InWorld, TArray<ABBBMonsterPresentationActor*> InActors)
            : Test(InTest), World(InWorld), Actors(MoveTemp(InActors))
        {
        }

        virtual bool Update() override
        {
            FEditorScriptExecutionGuard ScriptGuard;
            FBBBMonsterHitReactionFragment Hit;
            for (ABBBMonsterPresentationActor* Actor : Actors)
            {
                auto* Presentation = Actor->GetMonsterPresentation();
                if (!Presentation->BeginCorpsePresentation(FVector::ZeroVector, Hit, 3.0f, false))
                {
                    return false;
                }
                Test->TestTrue(TEXT("预算外尸体不能冻结在站姿"), Actor->GetMonsterMesh()->GetSocketLocation(TEXT("pelvis")).Z < 45.0f);
                Test->TestFalse(TEXT("预算外尸体不占实时物理"), Presentation->IsCorpseSimulating());
            }
            for (ABBBMonsterPresentationActor* Actor : Actors)
            {
                Actor->GetMonsterPresentation()->ResetCorpsePresentation();
            }
            GEngine->DestroyWorldContext(World);
            World->DestroyWorld(false);
            return true;
        }

    private:
        FAutomationTestBase* Test;
        UWorld* World;
        TArray<ABBBMonsterPresentationActor*> Actors;
    };
}

/** 验证十种外观的死亡物理 冻结姿势与池化清理边界 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterCorpseTest, "UBBB.Mass.ZombieCorpse",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterCorpseTest::RunTest(const FString& Parameters)
{
    FEditorScriptExecutionGuard ScriptGuard;
    TestNull(TEXT("移除旧死亡时长配置"), FindFProperty<FProperty>(UBBBMonsterDefinition::StaticClass(), TEXT("DeathLifetime")));
    TestNull(TEXT("生命片段移除旧死亡时长"), FindFProperty<FProperty>(FBBBMonsterHealthFragment::StaticStruct(), TEXT("DeathLifetime")));
    TestEqual(TEXT("默认保留尸体二十秒"), GetDefault<UBBBMonsterDefinition>()->CorpseLifetime, 20.0f);
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("尸体隔离物理世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    bool bWorldTransferred = false;
    ON_SCOPE_EXIT
    {
        if (!bWorldTransferred)
        {
            GEngine->DestroyWorldContext(World);
            World->DestroyWorld(false);
        }
    };
    TArray<ABBBMonsterPresentationActor*> HeldActors;
    const TArray<FString> Names = {TEXT("Connor"), TEXT("Jiho"), TEXT("Michael"), TEXT("Morita"), TEXT("Renzo"), TEXT("Sakurada"),
        TEXT("Alice"), TEXT("Kiyo"), TEXT("Setsuko"), TEXT("Yuina")};
    for (int32 Index = 0; Index < Names.Num(); ++Index)
    {
        const FString Gender = Index < 6 ? TEXT("Male") : TEXT("Female");
        const FString Root = TEXT("/Game/_Project/System/Mass/Monster/Zombie/") + Gender + TEXT("/Variants/") + Names[Index] + TEXT("/");
        const FString Name = TEXT("BP_BBBZombie") + Names[Index] + TEXT("Presentation");
        UClass* Class = LoadClass<ABBBMonsterPresentationActor>(nullptr, *(Root + Name + TEXT(".") + Name + TEXT("_C")));
        if (!TestNotNull(Names[Index], Class))
        {
            return false;
        }
        auto* Actor = World->SpawnActor<ABBBMonsterPresentationActor>(Class, FVector(Index * 250.0f, 0.0f, 90.0f), FRotator::ZeroRotator);
        if (!TestNotNull(TEXT("生成尸体测试演员"), Actor))
        {
            return false;
        }
        auto* Mesh = Actor->GetMonsterMesh();
        auto* Presentation = Actor->GetMonsterPresentation();
        const UPhysicsAsset* PhysicsAsset = Mesh->GetPhysicsAsset();
        if (!TestNotNull(TEXT("正式外观必须有尸体物理资产"), PhysicsAsset))
        {
            return false;
        }
        float TotalMass = 0.0f;
        for (const USkeletalBodySetup* Body : PhysicsAsset->SkeletalBodySetups)
        {
            TotalMass += Body ? Body->DefaultInstance.GetMassOverride() : 0.0f;
        }
        TestTrue(TEXT("尸体质量按人体部位分配 总量七十五公斤"), FMath::IsNearlyEqual(TotalMass, 75.0f, 0.01f));
        for (const UPhysicsConstraintTemplate* Constraint : PhysicsAsset->ConstraintSetup)
        {
            if (!TestTrue(TEXT("全部关节有独立尸体约束"), Constraint && Constraint->ContainsConstraintProfile(TEXT("BBBCorpse"))))
            {
                return false;
            }
            const auto& Profile = Constraint->GetConstraintProfilePropertiesOrDefault(TEXT("BBBCorpse"));
            TestFalse(TEXT("尸体关节不能使用软弹簧摆动"), Profile.ConeLimit.bSoftConstraint || Profile.TwistLimit.bSoftConstraint);
            TestTrue(TEXT("尸体不能拉长骨骼"), Profile.LinearLimit.XMotion == LCM_Locked && Profile.LinearLimit.YMotion == LCM_Locked && Profile.LinearLimit.ZMotion == LCM_Locked);
            TestFalse(TEXT("尸体关节不能持续驱动活体姿势"), Profile.AngularDrive.SlerpDrive.bEnablePositionDrive || Profile.AngularDrive.SwingDrive.bEnablePositionDrive || Profile.AngularDrive.TwistDrive.bEnablePositionDrive);
        }
        FBBBMonsterHitReactionFragment Hit;
        Hit.Serial = 1;
        Hit.Age = 0.0f;
        Hit.Position = Mesh->GetSocketLocation(TEXT("pelvis"));
        const FTransform Living = Mesh->GetRelativeTransform();
        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Dead, 0.0f, 1.0f, 1, 0.0f);
        TestTrue(TEXT("启用真实尸体物理"), Presentation->BeginCorpsePresentation(FVector(300.0f, 0.0f, 0.0f), Hit, 1.0f));
        TestTrue(TEXT("骨骼刚体进入模拟"), Mesh->IsSimulatingPhysics(TEXT("pelvis")));
        for (const FBodyInstance* Body : Mesh->Bodies)
        {
            TestTrue(TEXT("受击资产关闭重力也必须启用尸体重力"), Body && Body->bEnableGravity);
        }
        TestNull(TEXT("网格脱离活体位置控制"), Mesh->GetAttachParent());
        TestEqual(TEXT("尸体不阻挡玩家"), Mesh->GetCollisionResponseToChannel(ECC_Pawn), ECR_Ignore);
        TestFalse(TEXT("尸体不更新导航"), Mesh->CanEverAffectNavigation());
        Presentation->ApplyHitReaction(Hit);
        TestTrue(TEXT("受击重置不能关闭尸体物理"), Mesh->IsSimulatingPhysics(TEXT("pelvis")));
        const FVector Position = Mesh->GetComponentLocation();
        Presentation->ApplyMobilityState(true, 1.0f, 45.0f);
        TestEqual(TEXT("爬行偏移不能移动物理尸体"), Mesh->GetComponentLocation(), Position);
        Presentation->FreezeCorpsePresentation();
        TestTrue(TEXT("冻结仍保留尸体"), Presentation->IsCorpseActive());
        TestFalse(TEXT("冻结不再占用活动物理"), Presentation->IsCorpseSimulating());
        TestFalse(TEXT("冻结不评估动画"), Mesh->IsComponentTickEnabled());
        TestFalse(TEXT("冻结停止刚体模拟"), Mesh->IsSimulatingPhysics(TEXT("pelvis")));
        Presentation->ResetCorpsePresentation();
        TestFalse(TEXT("复用清除尸体标记"), Presentation->IsCorpseActive());
        TestTrue(TEXT("复用恢复网格挂接"), Mesh->GetAttachParent() == Actor->GetRootComponent());
        TestTrue(TEXT("复用恢复原始偏移"), Mesh->GetRelativeTransform().Equals(Living));
        TestFalse(TEXT("复用恢复动画"), Mesh->bPauseAnims);
        for (const FBodyInstance* Body : Mesh->Bodies)
        {
            const auto* Setup = Body ? Cast<USkeletalBodySetup>(Body->GetBodySetup()) : nullptr;
            TestTrue(TEXT("复用恢复活体刚体设置"), Setup && Body->bEnableGravity == Setup->DefaultInstance.bEnableGravity &&
                FMath::IsNearlyEqual(Body->LinearDamping, Setup->DefaultInstance.LinearDamping));
        }
        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Dead, 0.0f, 2.0f, 2, 1.0f);
        TestFalse(TEXT("预算外末帧等待真实动画帧"), Presentation->BeginCorpsePresentation(FVector::ZeroVector, Hit, 3.0f, false));
        HeldActors.Add(Actor);
    }
    bWorldTransferred = true;
    ADD_LATENT_AUTOMATION_COMMAND(FBBBMonsterCorpseHeldCommand(this, World, MoveTemp(HeldActors)));
    return true;
}

#endif
