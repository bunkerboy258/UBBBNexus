#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitReactionTarget.h"
#include "BBBMonsterGorePreviewComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "MassEntityConfigAsset.h"
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "PhysicsEngine/PhysicsAsset.h"

/** 只检查固定表现靶边界 不创建世界或启动游戏 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBZombieHitReactionTargetTest, "UBBB.Debug.ZombieHitReactionTarget",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBZombieHitReactionTargetTest::RunTest(const FString&)
{
    auto* Target = GetMutableDefault<ABBBZombieHitReactionTarget>();
    TestFalse(TEXT("固定靶没有移动更新"), Target->PrimaryActorTick.bCanEverTick);
    TestFalse(TEXT("固定靶不复制或创建玩法生命"), Target->GetIsReplicated());
    TestNotNull(TEXT("默认正式版本存在"), Target->ZombieVersion.Get());
    TestNotNull(TEXT("复用伤口表现"), Target->Gore.Get());
    TestNotNull(TEXT("复用正式动画桥接"), Target->AnimationPreview.Get());
    TestFalse(TEXT("默认不覆盖物理"), Target->bOverridePhysics);
    TestFalse(TEXT("默认不覆盖部位配置"), Target->bOverrideHitProfiles);
    TestFalse(TEXT("默认不覆盖伤口"), Target->bOverrideGore);
    TestEqual(TEXT("默认三秒自动恢复"), Target->GorePreview->ResetInterval, 3.0f);
    TestEqual(TEXT("子弹通道阻挡"), Target->TargetMesh->GetCollisionResponseToChannel(ECC_Pawn), ECR_Block);
    const FDamageEvent Damage;
    TestEqual(TEXT("非点伤害不会生成生命伤害"), Target->TakeDamage(MAX_flt, Damage, nullptr, nullptr), 0.0f);
    return true;
}

/** 在现有编辑器靶上逐版本解析并恢复原选择 不创建临时世界或保存资产 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBZombieHitReactionTargetVersionsTest, "UBBB.Debug.ZombieHitReactionTargetVersions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBZombieHitReactionTargetVersionsTest::RunTest(const FString&)
{
    UWorld* EditorWorld = nullptr;
    for (const FWorldContext& Context : GEngine->GetWorldContexts())
    {
        if (Context.WorldType == EWorldType::PIE)
        {
            AddError(TEXT("版本测试需要先结束用户 PIE"));
            return false;
        }
        if (Context.WorldType == EWorldType::Editor)
        {
            EditorWorld = Context.World();
        }
    }
    if (!TestNotNull(TEXT("已打开编辑器关卡"), EditorWorld))
    {
        return false;
    }
    ABBBZombieHitReactionTarget* Target = nullptr;
    for (TActorIterator<ABBBZombieHitReactionTarget> It(EditorWorld); It; ++It)
    {
        Target = *It;
        break;
    }
    if (!TestNotNull(TEXT("场景中存在固定靶"), Target))
    {
        return false;
    }
    const auto Version = Target->ZombieVersion;
    const auto Animation = Target->TestAnimation;
    const bool PhysicsOverride = Target->bOverridePhysics;
    const bool ProfilesOverride = Target->bOverrideHitProfiles;
    const bool GoreOverride = Target->bOverrideGore;
    const FTransform ActorTransform = Target->GetActorTransform();
    ON_SCOPE_EXIT
    {
        Target->ZombieVersion = Version;
        Target->TestAnimation = Animation;
        Target->bOverridePhysics = PhysicsOverride;
        Target->bOverrideHitProfiles = ProfilesOverride;
        Target->bOverrideGore = GoreOverride;
        Target->ApplyConfiguration();
    };
    Target->TestAnimation = nullptr;
    Target->bOverridePhysics = false;
    Target->bOverrideHitProfiles = false;
    Target->bOverrideGore = false;
    const TCHAR* Names[] = {TEXT("Connor"), TEXT("Jiho"), TEXT("Michael"), TEXT("Morita"), TEXT("Renzo"),
        TEXT("Sakurada"), TEXT("Alice"), TEXT("Kiyo"), TEXT("Setsuko"), TEXT("Yuina")};
    for (int32 Index = 0; Index < UE_ARRAY_COUNT(Names); ++Index)
    {
        const FString Path = FString::Printf(TEXT("/Game/_Project/Mass/Monster/Zombie/%s/Variants/%s/MEC_BBBZombie%s.MEC_BBBZombie%s"),
            Index < 6 ? TEXT("Male") : TEXT("Female"), Names[Index], Names[Index], Names[Index]);
        Target->ZombieVersion = LoadObject<UMassEntityConfigAsset>(nullptr, *Path);
        const bool bApplied = Target->ApplyConfiguration();
        if (!TestTrue(*FString::Printf(TEXT("版本完整解析 %s %s"), Names[Index], *Target->ConfigurationStatus), bApplied))
        {
            continue;
        }
        TestTrue(TEXT("应用完成状态有效"), Target->bConfigurationReady);
        TestEqual(TEXT("配对伤口身体一致"), Target->TargetMesh->GetSkeletalMeshAsset(), Target->GorePreview->Definition->Body.Get());
        TestEqual(TEXT("四种正式部位配置"), Target->HitReaction->AvailableProfiles.Num(), 4);
        TestEqual(TEXT("默认正式动画蓝图"), Target->TargetMesh->GetAnimationMode(), EAnimationMode::AnimationBlueprint);
        TestTrue(TEXT("版本切换不移动场景靶"), Target->GetActorTransform().Equals(ActorTransform));
        TestEqual(TEXT("版本切换恢复子弹碰撞"), Target->TargetMesh->GetCollisionEnabled(), ECollisionEnabled::QueryAndPhysics);
        TestEqual(TEXT("命中计数已清空"), Target->HitCount, 0u);
    }
    Target->bOverridePhysics = true;
    const auto PreviousPhysics = Target->PhysicsOverride;
    Target->PhysicsOverride = nullptr;
    TestFalse(TEXT("空物理覆盖明确拒绝"), Target->ApplyConfiguration());
    TestFalse(TEXT("失败后不能继续命中旧版本"), Target->bConfigurationReady);
    TestNull(TEXT("失败后清除旧身体"), Target->TargetMesh->GetSkeletalMeshAsset());
    Target->PhysicsOverride = PreviousPhysics;
    Target->bOverridePhysics = false;
    TestTrue(TEXT("撤销覆盖后可恢复"), Target->ApplyConfiguration());
    Target->bOverrideGore = true;
    const auto PreviousGore = Target->GoreOverride;
    Target->GoreOverride = nullptr;
    TestFalse(TEXT("空伤口覆盖明确拒绝"), Target->ApplyConfiguration());
    Target->GoreOverride = PreviousGore;
    Target->bOverrideGore = false;
    Target->bOverrideHitProfiles = true;
    const auto PreviousTorso = Target->TorsoProfile;
    Target->TorsoProfile = nullptr;
    TestFalse(TEXT("不完整部位覆盖明确拒绝"), Target->ApplyConfiguration());
    Target->TorsoProfile = PreviousTorso;
    Target->bOverrideHitProfiles = false;
    TestTrue(TEXT("最终可恢复正式版本"), Target->ApplyConfiguration());
    return true;
}

/** 只读检查场景靶头部与躯干的真实子弹通道 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBZombieHitReactionTargetCollisionTest, "UBBB.Debug.ZombieHitReactionTargetCollision",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBZombieHitReactionTargetCollisionTest::RunTest(const FString&)
{
    UWorld* EditorWorld = nullptr;
    for (const FWorldContext& Context : GEngine->GetWorldContexts())
    {
        if (Context.WorldType == EWorldType::Editor)
        {
            EditorWorld = Context.World();
        }
    }
    if (!TestNotNull(TEXT("需要已打开的编辑器关卡"), EditorWorld))
    {
        return false;
    }
    int32 Count = 0;
    for (TActorIterator<ABBBZombieHitReactionTarget> It(EditorWorld); It; ++It)
    {
        ++Count;
        TestTrue(TEXT("靶配置有效"), It->bConfigurationReady);
        for (const FName Bone : {FName(TEXT("spine_02")), FName(TEXT("head"))})
        {
            const FVector Position = It->TargetMesh->GetBoneLocation(Bone);
            const FVector Forward = It->GetActorForwardVector();
            FHitResult Hit;
            const bool bHit = EditorWorld->LineTraceSingleByChannel(Hit, Position + Forward * 200, Position - Forward * 50, ECC_Pawn);
            TestTrue(*FString::Printf(TEXT("真实子弹通道命中 %s"), *Bone.ToString()), bHit && Hit.GetActor() == *It);
            TestTrue(TEXT("命中目标骨骼网格"), Hit.GetComponent() == It->TargetMesh);
            TestFalse(TEXT("命中提供原始骨骼"), Hit.BoneName.IsNone());
        }
    }
    TestEqual(TEXT("关卡中一只调参靶"), Count, 1);
    return true;
}

#endif
