#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitReactionTarget.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/SkeletalMesh.h"
#include "HitReactProfile.h"
#include "Misc/AutomationTest.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterSeveringPresentationComponent.h"

/** 静态验证调参靶独立配置和纯表现边界 不创建世界或启动游戏 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBZombieHitReactionTargetTest, "UBBB.Debug.ZombieHitReactionTarget",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBZombieHitReactionTargetTest::RunTest(const FString&)
{
    auto* Target = GetMutableDefault<ABBBZombieHitReactionTarget>();
    TestFalse(TEXT("固定靶没有移动更新"), Target->PrimaryActorTick.bCanEverTick);
    TestFalse(TEXT("本地调参靶不产生网络实体"), Target->GetIsReplicated());
    TestNull(TEXT("不装配断肢表现组件"), Target->FindComponentByClass<UBBBMonsterSeveringPresentationComponent>());
    if (!TestNotNull(TEXT("受击复用正式组件"), Target->HitReaction.Get())
        || !TestNotNull(TEXT("具备骨骼碰撞"), Target->TargetMesh.Get()))
    {
        return false;
    }

    TestEqual(TEXT("完整查询和局部物理反应"), Target->TargetMesh->GetCollisionEnabled(), ECollisionEnabled::QueryAndPhysics);
    TestEqual(TEXT("不接入动画状态机"), Target->TargetMesh->GetAnimationMode(), EAnimationMode::AnimationSingleNode);
    const auto* Idle = Cast<UAnimSequence>(Target->TargetMesh->AnimationData.AnimToPlay);
    const auto* Mesh = Target->TargetMesh->GetSkeletalMeshAsset();
    if (TestNotNull(TEXT("待机序列可加载"), Idle) && TestNotNull(TEXT("目标网格可加载"), Mesh))
    {
        TestTrue(TEXT("待机与目标使用同一骨架"), Idle->GetSkeleton() == Mesh->GetSkeleton());
    }

    const auto* Physics = Target->TargetMesh->GetPhysicsAsset();
    if (TestNotNull(TEXT("专用物理资产可加载"), Physics))
    {
        TestEqual(TEXT("不编辑正式尸群物理资产"), Physics->GetPathName(),
            FString(TEXT("/Game/_Project/Debug/ZombieHitReaction/PA_ZombieHitReactionTarget.PA_ZombieHitReactionTarget")));
        const FName RequiredBones[] = {TEXT("spine_01"), TEXT("head"), TEXT("upperarm_l"),
            TEXT("upperarm_r"), TEXT("thigh_l"), TEXT("thigh_r")};
        for (const FName Bone : RequiredBones)
        {
            TestTrue(*FString::Printf(TEXT("调参部位具备物理体 %s"), *Bone.ToString()), Physics->FindBodyIndex(Bone) != INDEX_NONE);
        }
    }

    const TCHAR* Parts[] = {TEXT("Torso"), TEXT("Head"), TEXT("Arms"), TEXT("Legs")};
    if (!TestEqual(TEXT("四种配置顺序与正式组件一致"), Target->HitReaction->AvailableProfiles.Num(), 4))
    {
        return false;
    }
    for (int32 Index = 0; Index < UE_ARRAY_COUNT(Parts); ++Index)
    {
        auto Reference = Target->HitReaction->AvailableProfiles[Index];
        const auto* Profile = Reference.LoadSynchronous();
        if (TestNotNull(TEXT("调参部位配置可加载"), Profile))
        {
            TestEqual(TEXT("配置独立于正式尸群"), Profile->GetPathName(), FString::Printf(
                TEXT("/Game/_Project/Debug/ZombieHitReaction/HRP_Target%s.HRP_Target%s"), Parts[Index], Parts[Index]));
            TestEqual(TEXT("物理驱动沿用正式配置"), Profile->PhysicalAnimProfile, FName(TEXT("BBBHitReact")));
        }
    }

    const FDamageEvent Damage;
    TestEqual(TEXT("非点命中不生成生命伤害"), Target->TakeDamage(MAX_flt, Damage, nullptr, nullptr), 0.0f);
    TestEqual(TEXT("静态检查不注入游戏命中"), Target->HitCount, 0u);
    return true;
}

#endif
