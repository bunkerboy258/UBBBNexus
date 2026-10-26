#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/Package.h"
#include "BBBZombieHitReactionTarget.h"
#include "BBBMonsterGorePreviewComponent.h"
#include "BBBMonsterDestructionValidationLibrary.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGorePresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePointDamageEvent.h"
#include "Components/SkeletalMeshComponent.h"

/** 标准点伤害实际进入固定靶 再验证共享呈现与对象复用 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDestructionHitTest,
    "BBB.Monster.Gore.PointHitAndRecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDestructionHitTest::RunTest(const FString&)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("点命中隔离世界"), World))
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
    auto* Target = World->SpawnActor<ABBBZombieHitReactionTarget>();
    if (!TestNotNull(TEXT("正式固定靶"), Target) || !TestTrue(TEXT("真实配置可应用"), Target->ApplyConfiguration()))
    {
        return false;
    }
    Target->GorePreview->ResetInterval = 0;
    Target->GorePreview->HitsToSever = 1;
    Target->GorePreview->bEnablePhysicalReaction = false;
    auto* Mesh = Target->TargetMesh.Get();
    Mesh->TickAnimation(0, false);
    Mesh->RefreshBoneTransforms();
    const auto* Definition = Target->GorePreview->Definition.Get();
    const auto& Skeleton = Definition->Body->GetRefSkeleton();
    const auto HitAt = [&](const FName BoneName, const FVector& Contact, const FVector& Normal)
    {
        const int32 Bone = Skeleton.FindBoneIndex(BoneName);
        FTransform Reference = Skeleton.GetRefBonePose()[Bone];
        for (int32 Parent = Skeleton.GetParentIndex(Bone); Parent != INDEX_NONE; Parent = Skeleton.GetParentIndex(Parent))
        {
            Reference *= Skeleton.GetRefBonePose()[Parent];
        }
        const FTransform Current = Mesh->GetSocketTransform(BoneName);
        const FVector Position = Current.TransformPosition(Reference.InverseTransformPosition(Contact));
        const FVector WorldNormal = Current.TransformVectorNoScale(Reference.InverseTransformVectorNoScale(Normal)).GetSafeNormal();
        FBBBProjectilePointDamageEvent Event;
        Event.HitInfo = FHitResult(Target, Mesh, Position, WorldNormal);
        Event.HitInfo.ImpactPoint = Position;
        Event.HitInfo.ImpactNormal = WorldNormal;
        Event.HitInfo.BoneName = BoneName;
        Event.ShotDirection = -WorldNormal;
        Target->TakeDamage(1, Event, nullptr, nullptr);
    };
    for (int32 Index = 0; Index < 8; ++Index)
    {
        const auto& Region = Definition->Destruction->Regions[Index];
        for (int32 Hit = 0; Hit < 3; ++Hit)
        {
            HitAt(Region.Bone, Region.Contact, Region.Rotation.RotateVector(FVector::UpVector));
        }
        const auto& Parameters = Mesh->GetCustomPrimitiveData().Data;
        TestTrue(TEXT("三个实际点命中使对应区域完全毁伤"),
            Parameters.IsValidIndex(10 + Index) && FMath::IsNearlyEqual(Parameters[10 + Index], 1.0f));
    }
    for (int32 Region = 1; Region < 6; ++Region)
    {
        const auto* Part = Definition->Dismemberment->Parts.FindByPredicate([Region](const auto& Value)
        {
            return static_cast<int32>(Value.Region) == Region;
        });
        if (TestNotNull(TEXT("断点资源"), Part))
        {
            HitAt(Part->Bone, Part->Contact, FVector::ForwardVector);
        }
    }
    TArray<USkeletalMeshComponent*> Components;
    Target->GetComponents(Components);
    int32 Followers = 0;
    for (const auto* Component : Components)
    {
        if (Component == Mesh)
        {
            continue;
        }
        ++Followers;
        TestTrue(TEXT("组织共用身体姿态"), Component->LeaderPoseComponent.Get() == Mesh);
        TestFalse(TEXT("组织没有独立动画逐帧更新"), Component->PrimaryComponentTick.bCanEverTick);
        TestEqual(TEXT("组织不产生额外碰撞"), Component->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
    }
    TestEqual(TEXT("八区全开加五个断点最多六份组织组件"), Followers, 6);
    const auto& Parameters = Mesh->GetCustomPrimitiveData().Data;
    for (int32 Index = 0; Index < 5; ++Index)
    {
        TestTrue(TEXT("五处断点与躯干毁伤共同保留"), Parameters.IsValidIndex(1 + Index) && Parameters[1 + Index] >= 10);
    }
    Target->GorePreview->ResetTarget();
    for (int32 Index = 1; Index < 18; ++Index)
    {
        const auto& Cleared = Mesh->GetCustomPrimitiveData().Data;
        TestTrue(TEXT("复用清除全部破坏参数"), !Cleared.IsValidIndex(Index) || Cleared[Index] == 0);
    }
    for (const auto* Component : Components)
    {
        if (Component != Mesh)
        {
            TestFalse(TEXT("复用后组织隐藏"), Component->IsVisible());
        }
    }
    World->WorldType = EWorldType::PIE;
    const FString Inspection = UBBBMonsterDestructionValidationLibrary::InspectPopulationDestruction(World, {});
    World->WorldType = EWorldType::Game;
    TSharedPtr<FJsonObject> Report;
    TestTrue(TEXT("跨端空集合查询成功"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Inspection), Report));
    const TArray<TSharedPtr<FJsonValue>>* Rows = nullptr;
    TestTrue(TEXT("空世界查询返回实体数组"), Report.IsValid() && Report->TryGetArrayField(TEXT("entities"), Rows));
    TestTrue(TEXT("空世界查询返回零只小怪"), Rows && Rows->IsEmpty());
    return true;
}
#endif
