#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitReactionTarget.h"
#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitTrajectoryComponent.h"
#include "BBBMonsterGorePreviewComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGorePresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePointDamageEvent.h"

#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "HitReactProfile.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "UObject/ConstructorHelpers.h"
#include "MassEntityConfigAsset.h"
#include "MassVisualizationTrait.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Traits/BBBMonsterTrait.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
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

    static ConstructorHelpers::FObjectFinder<UMassEntityConfigAsset> Version(TEXT(
        "/Game/_Project/Mass/Monster/Zombie/Male/Variants/Connor/MEC_BBBZombieConnor.MEC_BBBZombieConnor"));
    ZombieVersion = Version.Object;

    PhysicalAnimation = CreateDefaultSubobject<UBBBMonsterPhysicalAnimationComponent>(TEXT("PhysicalAnimation"));
    PhysicalAnimation->PrimaryComponentTick.bStartWithTickEnabled = false;
    HitReaction = CreateDefaultSubobject<UBBBMonsterHitReactionComponent>(TEXT("HitReaction"));
    HitReaction->bAutoActivate = false;
    Trajectory = CreateDefaultSubobject<UBBBZombieHitTrajectoryComponent>(TEXT("Trajectory"));
    Gore = CreateDefaultSubobject<UBBBMonsterGorePresentationComponent>(TEXT("Gore"));
    GorePreview = CreateDefaultSubobject<UBBBMonsterGorePreviewComponent>(TEXT("GorePreview"));
    AnimationPreview = CreateDefaultSubobject<UBBBMonsterPresentationComponent>(TEXT("AnimationPreview"));
}

void ABBBZombieHitReactionTarget::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ApplyConfiguration();
}

#if WITH_EDITOR
void ABBBZombieHitReactionTarget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    if (GetWorld() && GetWorld()->IsGameWorld())
    {
        ApplyConfiguration();
    }
}
#endif

bool ABBBZombieHitReactionTarget::ApplyConfiguration()
{
    if (Trajectory->IsComponentTickEnabled())
    {
        ConfigurationStatus = TEXT("轨迹采样正在运行 请完成后应用配置");
        return false;
    }

    const auto Fail = [this](const FString& Reason)
    {
        bConfigurationReady = false;
        HitReaction->AvailableProfiles.Reset();
        if (TargetMesh->GetSkeletalMeshAsset())
        {
            HitReaction->Activate(true);
        }
        HitReaction->Deactivate();
        GorePreview->Definition = nullptr;
        GorePreview->ResetTarget();
        TargetMesh->SetSkeletalMesh(nullptr);
        TargetMesh->SetVisibility(false);
        TargetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        ConfigurationStatus = Reason;
        return false;
    };

    if (!ZombieVersion)
    {
        return Fail(TEXT("请选择正式僵尸实体模板"));
    }
    const auto* Trait = Cast<UBBBMonsterTrait>(ZombieVersion->FindTrait(UBBBMonsterTrait::StaticClass()));
    if (!Trait || !Trait->Definition)
    {
        return Fail(TEXT("模板缺少小怪定义或正式高精度表现类"));
    }
    const auto* Settings = bOverridePresentation ? PresentationOverride.Get() : Trait->Definition->Presentation.Get();
    if (!Settings || !Settings->IsValid())
    {
        return Fail(TEXT("所选表现配置不完整"));
    }
    auto* Wounds = Settings->Gore.Get();
    auto* Body = Wounds->Body.Get();
    auto* Physics = Settings->PhysicsAsset.Get();
    if (bOverridePresentation && (!Trait->Definition->Presentation
        || Trait->Definition->Presentation->Gore->Body != Body))
    {
        return Fail(TEXT("自定义表现身体必须与所选僵尸版本一致"));
    }
    if (!Wounds || !Wounds->IsReady() || Wounds->Body != Body)
    {
        return Fail(TEXT("伤口配置不完整或不属于所选版本的身体"));
    }
    if (!Physics)
    {
        return Fail(TEXT("所选版本或物理覆盖缺少物理资产"));
    }
    const FName RequiredBones[] = {TEXT("spine_01"), TEXT("head"), TEXT("upperarm_l"),
        TEXT("upperarm_r"), TEXT("thigh_l"), TEXT("thigh_r")};
    for (const FName Bone : RequiredBones)
    {
        if (Body->GetRefSkeleton().FindBoneIndex(Bone) == INDEX_NONE || Physics->FindBodyIndex(Bone) == INDEX_NONE)
        {
            return Fail(FString::Printf(TEXT("身体或物理资产缺少受击骨骼 %s"), *Bone.ToString()));
        }
    }
    if (TestAnimation && (!TestAnimation->GetSkeleton() || TestAnimation->GetSkeleton() != Body->GetSkeleton()))
    {
        return Fail(TEXT("测试动画与所选身体骨架不兼容"));
    }
    if (!TestAnimation && !Settings->AnimationClass)
    {
        return Fail(TEXT("正式表现类缺少动画蓝图 请明确选择兼容的测试动画"));
    }
    TArray<TSoftObjectPtr<UHitReactProfile>> Profiles = {Settings->HitReaction.TorsoProfile.Get(), Settings->HitReaction.HeadProfile.Get(),
        Settings->HitReaction.ArmsProfile.Get(), Settings->HitReaction.LegsProfile.Get()};
    if (Profiles.Num() != 4)
    {
        return Fail(TEXT("程序化受击必须提供躯干 头部 手臂 腿部四份配置"));
    }
    for (auto& Profile : Profiles)
    {
        if (!Profile.LoadSynchronous())
        {
            return Fail(TEXT("程序化受击存在空引用或无法加载的配置"));
        }
    }

    bConfigurationReady = false;
    HitReaction->Deactivate();
    GorePreview->Definition = nullptr;
    GorePreview->ResetTarget();
    TargetMesh->EmptyOverrideMaterials();
    TargetMesh->SetSkeletalMesh(Body);
    TargetMesh->SetPhysicsAsset(Physics, true);
    TargetMesh->SetAllBodiesSimulatePhysics(false);
    TargetMesh->SetAllBodiesPhysicsBlendWeight(0);
    TargetMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
    TargetMesh->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector);
    FTransform Relative = Settings->MeshTransform;
    // 正式演员原点位于胶囊中心 固定靶原点位于地面
    Relative.AddToTranslation(FVector(0, 0, Trait->Definition->CapsuleHalfHeight));
    TargetMesh->SetRelativeTransform(Relative);
    HitReaction->AvailableProfiles = MoveTemp(Profiles);
    HitReaction->ConfigurePresentation(Settings);
    TargetMesh->AnimationData = FSingleAnimationPlayData();
    TargetMesh->SetAnimInstanceClass(nullptr);
    if (TestAnimation)
    {
        TargetMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
        TargetMesh->AnimationData.AnimToPlay = TestAnimation;
        TargetMesh->AnimationData.bSavedLooping = true;
        TargetMesh->AnimationData.bSavedPlaying = true;
        TargetMesh->PlayAnimation(TestAnimation, true);
    }
    if (!TestAnimation)
    {
        TargetMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        TargetMesh->SetAnimInstanceClass(Settings->AnimationClass);
    }
    HitCount = 0;
    LastHitBone = NAME_None;
    LastHitRegion = EBBBMonsterHitRegion::Torso;
    GorePreview->Definition = Wounds;
    TargetMesh->SetVisibility(true);
    TargetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    bConfigurationReady = true;
    UpdatePreviewAnimation();
    GorePreview->ResetTarget();
    if (GetWorld() && GetWorld()->IsGameWorld())
    {
        HitReaction->Activate(true);
    }
    ConfigurationStatus = FString::Printf(TEXT("已应用 %s | 表现:%s | 动画:%s"),
        *ZombieVersion->GetName(), *Settings->GetName(),
        TestAnimation ? *TestAnimation->GetName() : TEXT("正式待机蓝图"));
    return true;
}

void ABBBZombieHitReactionTarget::UpdatePreviewAnimation()
{
    if (!bConfigurationReady || !GetWorld() || TestAnimation || !TargetMesh->GetAnimInstance())
    {
        return;
    }
    FBBBMonsterVariationFragment Variation;
    Variation.Seed = 17;
    const auto* Settings = GorePreview->Definition->GetTypedOuter<UBBBMonsterPresentationDefinition>();
    AnimationPreview->ApplyVariationFacts(Variation, FMath::Fmod(GetWorld()->GetTimeSeconds()
        / Settings->Animation.IdleCycleSeconds[Variation.Seed % 3], 1.0f));
    AnimationPreview->ApplyPresentationState(EBBBMonsterBehavior::Idle, 0, 0, 0, 0);
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
    if (!bConfigurationReady || !GetWorld() || !GetWorld()->IsGameWorld() || DamageAmount <= 0.0f
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
    if (GorePreview->bEnablePhysicalReaction)
    {
        HitReaction->ApplyHitFacts(Hit, true);
    }
    const float Strength = DamageEvent.IsOfType(FBBBProjectilePointDamageEvent::ClassID)
        ? static_cast<const FBBBProjectilePointDamageEvent&>(DamageEvent).WoundStrength : 1.0f;
    GorePreview->ApplyHit(Hit, Strength);
    return 0.0f;
}
