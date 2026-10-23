#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitReactionTarget.h"
#include "BBBMonsterGorePreviewComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HitReactProfile.h"
#include "Misc/AutomationTest.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"

/** 静态验证调参靶独立配置和纯表现边界 不创建世界或启动游戏 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBZombieHitReactionTargetTest, "UBBB.Debug.ZombieHitReactionTarget",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBZombieHitReactionTargetTest::RunTest(const FString&)
{
    auto* Target = GetMutableDefault<ABBBZombieHitReactionTarget>();
    TestFalse(TEXT("固定靶没有移动更新"), Target->PrimaryActorTick.bCanEverTick);
    TestFalse(TEXT("本地调参靶不产生网络实体"), Target->GetIsReplicated());
    TestNotNull(TEXT("伤口断肢复用正式表现"), Target->Gore.Get());
    TestNotNull(TEXT("血腥预览输入存在"), Target->GorePreview.Get());
    TestEqual(TEXT("调参靶默认每三秒恢复"), Target->GorePreview->ResetInterval, 3.0f);
    if (!TestNotNull(TEXT("受击复用正式组件"), Target->HitReaction.Get())
        || !TestNotNull(TEXT("具备骨骼碰撞"), Target->TargetMesh.Get()))
    {
        return false;
    }

    TestEqual(TEXT("完整查询和局部物理反应"), Target->TargetMesh->GetCollisionEnabled(), ECollisionEnabled::QueryAndPhysics);
    TestEqual(TEXT("调参靶必须阻挡正式子弹的 Pawn 查询通道"), Target->TargetMesh->GetCollisionResponseToChannel(ECC_Pawn), ECR_Block);
    TestEqual(TEXT("网格持有局部物理位置求解次数"), Target->TargetMesh->BodyInstance.GetPositionSolverIterationCount(), 32);
    TestEqual(TEXT("网格持有局部物理速度求解次数"), Target->TargetMesh->BodyInstance.GetVelocitySolverIterationCount(), 8);
    TestEqual(TEXT("网格持有关节投影求解次数"), Target->TargetMesh->BodyInstance.GetProjectionSolverIterationCount(), 4);
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

/** 在当前编辑器关卡直接检查真实子弹通道 不创建世界或启动游戏 */
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
            break;
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
        const FName Bones[] = {TEXT("spine_02"), TEXT("head")};
        for (const FName Bone : Bones)
        {
            const FVector Position = It->TargetMesh->GetBoneLocation(Bone);
            const FVector Forward = It->GetActorForwardVector();
            FHitResult Hit;
            const bool bHit = EditorWorld->LineTraceSingleByChannel(Hit,
                Position + Forward * 200.0f, Position - Forward * 50.0f, ECC_Pawn);
            TestTrue(*FString::Printf(TEXT("真实子弹通道命中调参靶 %s"), *Bone.ToString()), bHit && Hit.GetActor() == *It);
            TestTrue(TEXT("点命中落在目标骨骼网格"), Hit.GetComponent() == It->TargetMesh);
            TestFalse(TEXT("命中具备可用于部位识别的骨骼"), Hit.BoneName.IsNone());
        }
    }
    TestEqual(TEXT("关卡中只有一只调参靶"), Count, 1);
    return true;
}

#endif
