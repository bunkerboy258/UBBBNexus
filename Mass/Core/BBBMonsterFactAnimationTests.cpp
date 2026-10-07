#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterFactAnimInstance.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBudgetPresentationActor.h"
#include "SkeletalMeshComponentBudgeted.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/UnrealType.h"
#include "Engine/SkeletalMesh.h"
#include "MassEntityConfigAsset.h"
#include "MassVisualizationTrait.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Traits/BBBMonsterTrait.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"

/** 验证男女僵尸的新基类只复制事实 不选择动画或推进玩法时间 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterFactAnimationTest, "UBBB.Mass.ZombieAnimationFacts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterFactAnimationTest::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("六行为死亡枚举值"), static_cast<int32>(EBBBMonsterBehavior::Dead), 5);
    TestEqual(TEXT("行为枚举只包含六种行为与引擎末尾标记"), StaticEnum<EBBBMonsterBehavior>()->NumEnums(), 7);
    TestNull(TEXT("定义不保留硬直时长"), FindFProperty<FProperty>(UBBBMonsterDefinition::StaticClass(), TEXT("HurtDuration")));
    TestNull(TEXT("生命片段不保留硬直时长"), FindFProperty<FProperty>(FBBBMonsterHealthFragment::StaticStruct(), TEXT("HurtDuration")));
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false)
        .CreatePhysicsScene(false)
        .CreateNavigation(false)
        .CreateAISystem(false)
        .ShouldSimulatePhysics(false);
    UWorld* const World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("事实动画测试世界"), World))
    {
        return false;
    }

    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);

    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };

    const TArray<FString> Classes =
    {
        TEXT("/Game/_Project/System/Mass/Monster/Zombie/Male/BP_BBBZombieMalePresentation.BP_BBBZombieMalePresentation_C"),
        TEXT("/Game/_Project/System/Mass/Monster/Zombie/Female/BP_BBBZombieFemalePresentation.BP_BBBZombieFemalePresentation_C")
    };

    for (const FString& ClassPath : Classes)
    {
        UClass* const ActorClass = LoadClass<ABBBMonsterPresentationActor>(nullptr, *ClassPath);
        if (!TestNotNull(ClassPath, ActorClass))
        {
            return false;
        }

        ABBBMonsterPresentationActor* const Actor = World->SpawnActor<ABBBMonsterPresentationActor>(ActorClass);
        if (!TestNotNull(TEXT("僵尸表现演员"), Actor))
        {
            return false;
        }

        UBBBMonsterFactAnimInstance* const Animation = Cast<UBBBMonsterFactAnimInstance>(Actor->GetMonsterMesh()->GetAnimInstance());
        TestNotNull(TEXT("正式僵尸必须使用独立预算载体"), Cast<ABBBMonsterBudgetPresentationActor>(Actor));
        TestNotNull(TEXT("正式僵尸网格必须接入引擎预算"), Cast<USkeletalMeshComponentBudgeted>(Actor->GetMonsterMesh()));
        if (!TestNotNull(TEXT("僵尸必须使用独立事实基类"), Animation))
        {
            return false;
        }

        const FFloatProperty* const Speed = FindFProperty<FFloatProperty>(Animation->GetClass(), TEXT("MovementSpeedFact"));
        const FFloatProperty* const Progress = FindFProperty<FFloatProperty>(Animation->GetClass(), TEXT("ActionProgressFact"));
        const FFloatProperty* const Entered = FindFProperty<FFloatProperty>(Animation->GetClass(), TEXT("StateEnteredTimeFact"));
        const FInt64Property* const ActionId = FindFProperty<FInt64Property>(Animation->GetClass(), TEXT("ActionIdFact"));
        const FEnumProperty* const Behavior = FindFProperty<FEnumProperty>(Animation->GetClass(), TEXT("BehaviorFact"));
        if (!TestTrue(TEXT("五个只读事实必须可反射"), Speed && Progress && Entered && ActionId && Behavior))
        {
            return false;
        }

        TestTrue(TEXT("事实属性禁止蓝图写入"), Speed->HasAllPropertyFlags(CPF_BlueprintReadOnly) && Progress->HasAllPropertyFlags(CPF_BlueprintReadOnly));
        TestTrue(TEXT("动画蓝图必须拥有正式状态机"), Animation->GetStateMachineIndex(TEXT("FactDrivenActions")) != INDEX_NONE);
        TestTrue(TEXT("动画蓝图必须拥有持续爬行姿势分支"), Animation->GetStateMachineIndex(TEXT("FactDrivenCrawl")) != INDEX_NONE);
        const FBoolProperty* Crawl = FindFProperty<FBoolProperty>(Animation->GetClass(), TEXT("CrawlingFact"));
        const FFloatProperty* CrawlProgress = FindFProperty<FFloatProperty>(Animation->GetClass(), TEXT("CrawlProgressFact"));
        TestTrue(TEXT("爬行事实只读且可反射"), Crawl && CrawlProgress && Crawl->HasAllPropertyFlags(CPF_BlueprintReadOnly) && CrawlProgress->HasAllPropertyFlags(CPF_BlueprintReadOnly));
        TestNull(TEXT("事实基类没有旧资产查询"), Animation->GetClass()->FindFunctionByName(TEXT("GetActiveAnimation")));
        TestNull(TEXT("动画蓝图不保留硬直姿势索引"), FindFProperty<FProperty>(Animation->GetClass(), TEXT("HurtPoseIndex")));
        TestNull(TEXT("动画蓝图不保留硬直采样时间"), FindFProperty<FProperty>(Animation->GetClass(), TEXT("HurtSampleTime")));
        const FIntProperty* const Epoch = FindFProperty<FIntProperty>(Animation->GetClass(), TEXT("ActionEpoch"));
        const FIntProperty* const PoseIndex = FindFProperty<FIntProperty>(Animation->GetClass(), TEXT("AttackPoseIndex"));
        const FFloatProperty* const SampleTime = FindFProperty<FFloatProperty>(Animation->GetClass(), TEXT("AttackSampleTime"));
        if (!TestTrue(TEXT("动画蓝图必须维护姿势槽与采样时间"), Epoch && PoseIndex && SampleTime))
        {
            return false;
        }

        UBBBMonsterPresentationComponent* const Presentation = Actor->GetMonsterPresentation();
        Presentation->ApplyMobilityState(true, 1.0f, 45.0f);
        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Chase, 75.0f, 0.0f, 0, 0.0f);
        Animation->NativeUpdateAnimation(0.016f);
        TestTrue(TEXT("持续爬行快照准确复制"), Crawl->GetPropertyValue_InContainer(Animation));
        TestEqual(TEXT("倒地进度只读复制"), CrawlProgress->GetPropertyValue_InContainer(Animation), 1.0f);
        TestEqual(TEXT("表现脚底偏移随胶囊降低"), Actor->GetMonsterMesh()->GetRelativeLocation().Z, -45.0);
        Presentation->ApplyMobilityState(false, 0.0f, 90.0f);
        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Chase, 180.0f, 1.0f, 7, 0.0f);
        Animation->NativeUpdateAnimation(0.01f);
        TestEqual(TEXT("实际速度原样复制"), Speed->GetPropertyValue_InContainer(Animation), 180.0f);
        TestEqual(TEXT("进入时间原样复制"), Entered->GetPropertyValue_InContainer(Animation), 1.0f);
        TestEqual(TEXT("动作编号原样复制"), ActionId->GetPropertyValue_InContainer(Animation), static_cast<int64>(7));

        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Attack, 0.0f, 2.0f, 8, 0.4f);
        Animation->NativeUpdateAnimation(0.2f);
        Animation->BlueprintThreadSafeUpdateAnimation(0.2f);
        const int32 PreviousEpoch = Epoch->GetPropertyValue_InContainer(Animation);
        const int32 PreviousPoseIndex = PoseIndex->GetPropertyValue_InContainer(Animation);
        TestTrue(TEXT("线程安全蓝图按事实计算动作时间"), SampleTime->GetPropertyValue_InContainer(Animation) > 0.0f);
        TestEqual(TEXT("攻击进度不由帧时间生成"), Progress->GetPropertyValue_InContainer(Animation), 0.4f);
        Animation->NativeUpdateAnimation(1.0f);
        Animation->BlueprintThreadSafeUpdateAnimation(1.0f);
        TestEqual(TEXT("重复快照不得反复切换姿势槽"), Epoch->GetPropertyValue_InContainer(Animation), PreviousEpoch);
        TestEqual(TEXT("无新快照时进度不能自行推进"), Progress->GetPropertyValue_InContainer(Animation), 0.4f);

        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Attack, 0.0f, 3.0f, 9, 0.0f);
        Animation->NativeUpdateAnimation(0.01f);
        Animation->BlueprintThreadSafeUpdateAnimation(0.01f);
        TestEqual(TEXT("同状态新动作必须切换姿势槽"), Epoch->GetPropertyValue_InContainer(Animation), 1 - PreviousEpoch);
        TestNotEqual(TEXT("同状态重启必须请求不同惯性子姿势"), PoseIndex->GetPropertyValue_InContainer(Animation), PreviousPoseIndex);
        TestEqual(TEXT("线程安全蓝图必须同步归零采样时间"), SampleTime->GetPropertyValue_InContainer(Animation), 0.0f);
        TestEqual(TEXT("同状态新动作编号可见"), ActionId->GetPropertyValue_InContainer(Animation), static_cast<int64>(9));
        TestEqual(TEXT("同状态新动作进度归零"), Progress->GetPropertyValue_InContainer(Animation), 0.0f);

        Presentation->ApplyPresentationState(EBBBMonsterBehavior::Dead, 0.0f, 4.0f, 10, 1.0f);
        Animation->NativeUpdateAnimation(0.01f);
        TestEqual(TEXT("死亡末帧由事实决定"), Progress->GetPropertyValue_InContainer(Animation), 1.0f);
        const void* const BehaviorAddress = Behavior->ContainerPtrToValuePtr<void>(Animation);
        TestEqual(TEXT("死亡枚举原样复制"), Behavior->GetUnderlyingProperty()->GetUnsignedIntPropertyValue(BehaviorAddress), static_cast<uint64>(EBBBMonsterBehavior::Dead));
        TestTrue(TEXT("事实动画不能移动世界位置"), Actor->GetActorLocation().IsNearlyZero());
        Actor->Destroy();
    }

    return true;
}

/** 验证正式十种外观的网格构建与独立实体模板引用 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterAppearanceVariantsTest, "UBBB.Mass.ZombieAppearanceVariants",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterAppearanceVariantsTest::RunTest(const FString& Parameters)
{
    const TArray<FString> Names =
    {
        TEXT("Connor"), TEXT("Jiho"), TEXT("Michael"), TEXT("Morita"), TEXT("Renzo"), TEXT("Sakurada"),
        TEXT("Alice"), TEXT("Kiyo"), TEXT("Setsuko"), TEXT("Yuina")
    };

    TSet<const USkeletalMesh*> UniqueMeshes;
    TSet<const UMassEntityConfigAsset*> UniqueConfigs;
    for (int32 Index = 0; Index < Names.Num(); ++Index)
    {
        const FString& Name = Names[Index];
        const FString Gender = Index < 6 ? TEXT("Male") : TEXT("Female");
        const FString Directory = TEXT("/Game/_Project/System/Mass/Monster/Zombie/") + Gender + TEXT("/Variants/") + Name + TEXT("/");
        UClass* const ActorClass = LoadClass<ABBBMonsterBudgetPresentationActor>(nullptr, *(Directory + TEXT("BP_BBBZombie") + Name + TEXT("Presentation.BP_BBBZombie") + Name + TEXT("Presentation_C")));
        USkeletalMesh* const Mesh = LoadObject<USkeletalMesh>(nullptr, *(Directory + TEXT("SKM_BBBZombie") + Name));
        UBBBMonsterDefinition* const Definition = LoadObject<UBBBMonsterDefinition>(nullptr, *(Directory + TEXT("DA_BBBZombie") + Name + TEXT("Definition")));
        UMassEntityConfigAsset* const Config = LoadObject<UMassEntityConfigAsset>(nullptr, *(Directory + TEXT("MEC_BBBZombie") + Name));
        if (!TestTrue(Name + TEXT(" 四个正式资产必须可载入"), ActorClass && Mesh && Definition && Config))
        {
            return false;
        }

        const ABBBMonsterBudgetPresentationActor* const Actor = ActorClass->GetDefaultObject<ABBBMonsterBudgetPresentationActor>();
        const UBBBMonsterTrait* const MonsterTrait = Cast<UBBBMonsterTrait>(Config->FindTrait(UBBBMonsterTrait::StaticClass()));
        const UMassVisualizationTrait* const VisualTrait = Cast<UMassVisualizationTrait>(Config->FindTrait(UMassVisualizationTrait::StaticClass()));
        TestNotNull(Name + TEXT(" 原始构图数据必须存在"), Mesh->GetMeshDescription(0));
        TestEqual(Name + TEXT(" 必须具备三层 LOD"), Mesh->GetLODNum(), 3);
        TestEqual(Name + TEXT(" 逆参考矩阵必须完整"), Mesh->GetRefBasesInvMatrix().Num(), Mesh->GetRefSkeleton().GetNum());
        TestTrue(Name + TEXT(" 默认载体必须引用本变体网格"), Actor->GetMonsterMesh()->GetSkeletalMeshAsset() == Mesh);
        TestTrue(Name + TEXT(" 默认载体必须使用预算网格"), Actor->GetMonsterMesh()->IsA<USkeletalMeshComponentBudgeted>());
        UClass* const AnimationClass = Actor->GetMonsterMesh()->GetAnimClass();
        TestTrue(Name + TEXT(" 动画必须来自事实基类"), AnimationClass && AnimationClass->IsChildOf(UBBBMonsterFactAnimInstance::StaticClass()));
        TestTrue(Name + TEXT(" 定义必须指向本模板"), Definition->EntityConfig == Config);
        TestTrue(Name + TEXT(" 模板必须指向本定义"), MonsterTrait && MonsterTrait->Definition == Definition);
        TestTrue(Name + TEXT(" 模板必须指向本载体"), VisualTrait && VisualTrait->HighResTemplateActor == ActorClass && VisualTrait->LowResTemplateActor == ActorClass);
        UniqueMeshes.Add(Mesh);
        UniqueConfigs.Add(Config);
    }

    TestEqual(TEXT(" 十种外观不能重复引用同一网格"), UniqueMeshes.Num(), Names.Num());
    TestEqual(TEXT(" 十种外观必须有独立实体模板"), UniqueConfigs.Num(), Names.Num());
    return true;
}

#endif
