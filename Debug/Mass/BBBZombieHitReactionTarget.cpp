#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitReactionTarget.h"
#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitTrajectoryComponent.h"
#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieGoreDebugComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterSeveringPresentationComponent.h"

#include "Animation/AnimSequence.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "HitReactProfile.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "UObject/ConstructorHelpers.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPhysicalAnimationComponent.h"

ABBBZombieHitReactionTarget::ABBBZombieHitReactionTarget()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetCanBeDamaged(true);
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("FixedRoot")));

    TargetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TargetMesh"));
    TargetMesh->SetupAttachment(RootComponent);
    TargetMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    TargetMesh->SetCollisionObjectType(ECC_Pawn);
    TargetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    TargetMesh->SetCollisionResponseToAllChannels(ECR_Block);
    // 网格组件持有整条局部物理链的求解覆盖 避免只改刚体模板而未实际生效
    TargetMesh->BodyInstance.SetOverrideIterationCounts(true);
    TargetMesh->BodyInstance.SetPositionSolverIterationCount(32);
    TargetMesh->BodyInstance.SetVelocitySolverIterationCount(8);
    TargetMesh->BodyInstance.SetProjectionSolverIterationCount(4);
    // 正式子弹使用 Pawn 查询通道 必须阻挡才能收到标准点命中
    TargetMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    TargetMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    TargetMesh->SetGenerateOverlapEvents(false);
    TargetMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT(
        "/Game/_Project/Mass/Monster/Zombie/Male/Variants/Connor/SKM_BBBZombieConnor.SKM_BBBZombieConnor"));
    static ConstructorHelpers::FObjectFinder<UPhysicsAsset> PhysicsAsset(TEXT(
        "/Game/_Project/Debug/ZombieHitReaction/PA_ZombieHitReactionTarget.PA_ZombieHitReactionTarget"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> IdleAsset(TEXT(
        "/Game/_Project/Mass/Monster/Zombie/Male/Animations/Ramster/Zombie_Idle_01.Zombie_Idle_01"));
    TargetMesh->SetSkeletalMesh(MeshAsset.Object);
    TargetMesh->SetPhysicsAsset(PhysicsAsset.Object);
    TargetMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    TargetMesh->AnimationData.AnimToPlay = IdleAsset.Object;
    TargetMesh->AnimationData.bSavedLooping = true;
    TargetMesh->AnimationData.bSavedPlaying = true;
    TargetMesh->AnimationData.SavedPlayRate = 1.0f;

    PhysicalAnimation = CreateDefaultSubobject<UBBBMonsterPhysicalAnimationComponent>(TEXT("PhysicalAnimation"));
    PhysicalAnimation->PrimaryComponentTick.bStartWithTickEnabled = false;
    HitReaction = CreateDefaultSubobject<UBBBMonsterHitReactionComponent>(TEXT("HitReaction"));
    Trajectory = CreateDefaultSubobject<UBBBZombieHitTrajectoryComponent>(TEXT("Trajectory"));
    GorePresentation = CreateDefaultSubobject<UBBBMonsterSeveringPresentationComponent>(TEXT("GorePresentation"));
    GoreDebug = CreateDefaultSubobject<UBBBZombieGoreDebugComponent>(TEXT("GoreDebug"));
    HitReaction->AvailableProfiles.Reset();
    const TCHAR* Parts[] = {TEXT("Torso"), TEXT("Head"), TEXT("Arms"), TEXT("Legs")};
    for (const TCHAR* Part : Parts)
    {
        HitReaction->AvailableProfiles.Add(TSoftObjectPtr<UHitReactProfile>(FSoftObjectPath(FString::Printf(
            TEXT("/Game/_Project/Debug/ZombieHitReaction/HRP_Target%s.HRP_Target%s"), Part, Part))));
    }
}

FString ABBBZombieHitReactionTarget::StartHitTrajectory(const FString& RequestJson)
{
    return Trajectory->Start(RequestJson);
}

FString ABBBZombieHitReactionTarget::GetHitTrajectoryStatus() const
{
    return Trajectory->Status();
}

float ABBBZombieHitReactionTarget::TakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (!GetWorld() || !GetWorld()->IsGameWorld() || DamageAmount <= 0.0f
        || !DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        return 0.0f;
    }

    const auto& Point = static_cast<const FPointDamageEvent&>(DamageEvent);
    if (Point.HitInfo.GetComponent() != TargetMesh || Point.ShotDirection.ContainsNaN()
        || Point.ShotDirection.IsNearlyZero() || Point.HitInfo.ImpactPoint.ContainsNaN())
    {
        return 0.0f;
    }

    LastHitBone = Point.HitInfo.BoneName;
    LastHitRegion = EBBBMonsterHitRegion::Torso;
    for (FName Bone = LastHitBone; !Bone.IsNone(); Bone = TargetMesh->GetParentBone(Bone))
    {
        if (Bone == TEXT("head") || Bone == TEXT("neck_01"))
        {
            LastHitRegion = EBBBMonsterHitRegion::Head;
            break;
        }
        if (Bone == TEXT("upperarm_l"))
        {
            LastHitRegion = EBBBMonsterHitRegion::LeftArm;
            break;
        }
        if (Bone == TEXT("upperarm_r"))
        {
            LastHitRegion = EBBBMonsterHitRegion::RightArm;
            break;
        }
        if (Bone == TEXT("thigh_l"))
        {
            LastHitRegion = EBBBMonsterHitRegion::LeftLeg;
            break;
        }
        if (Bone == TEXT("thigh_r"))
        {
            LastHitRegion = EBBBMonsterHitRegion::RightLeg;
            break;
        }
    }

    ++HitCount;
    if (HitCount == 0)
    {
        ++HitCount;
    }
    FBBBMonsterHitReactionFragment Hit;
    Hit.Serial = HitCount;
    Hit.Age = 0.0f;
    Hit.Region = LastHitRegion;
    Hit.Position = Point.HitInfo.ImpactPoint;
    Hit.Direction = Point.ShotDirection.GetSafeNormal();
    Hit.Normal = Point.HitInfo.ImpactNormal;
    if (GoreDebug->bEnablePhysicalReaction)
    {
        HitReaction->ApplyHitFacts(Hit, true);
    }
    GoreDebug->ApplyHit(Hit);
    return 0.0f;
}
