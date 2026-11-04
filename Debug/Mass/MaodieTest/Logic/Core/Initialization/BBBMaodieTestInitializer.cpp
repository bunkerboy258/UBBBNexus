#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/Core/Initialization/BBBMaodieTestInitializer.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Network/BBBMaodieTestNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Animation/AnimSequence.h"
#include "Engine/SkeletalMesh.h"
void FBBBMaodieTestInitializer::Construct(ABBBMaodieTest &Actor)
{
    Actor.PrimaryActorTick.bCanEverTick = true;
    Actor.PrimaryActorTick.TickInterval = 0.02f;
    Actor.bReplicates = true;
    Actor.SetReplicateMovement(true);
    Actor.AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    Actor.AIControllerClass = AAIController::StaticClass();
    Actor.bUseControllerRotationYaw = false;
    Actor.GetCapsuleComponent()->InitCapsuleSize(35.0f, 58.0f);
    Actor.GetCharacterMovement()->bOrientRotationToMovement = true;
    Actor.GetCharacterMovement()->RotationRate = FRotator(0.0f, 240.0f, 0.0f);
    Actor.GetCharacterMovement()->NavAgentProps.AgentRadius = 35.0f;
    Actor.GetCharacterMovement()->NavAgentProps.AgentHeight = 116.0f;
    Actor.GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -58.0f));
    Actor.GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    Actor.GetMesh()->SetRelativeScale3D(FVector(100.0f));
    Actor.GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Actor.GetMesh()->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/_Project/Test/Maodie/Source/Model/Meshes/MaoDie.MaoDie")));
    Actor.Config.WalkAnimation = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_Project/Test/Maodie/Source/Model/Animations/AN_Monster_Chase_Anim.AN_Monster_Chase_Anim"));
    Actor.Config.IdleAnimation = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_Project/Test/Maodie/Source/Model/Animations/AN_Monster_Idle_Anim.AN_Monster_Idle_Anim"));
    Actor.Network = Actor.CreateDefaultSubobject<UBBBMaodieTestNetworkComponent>(TEXT("MaodieNetwork"));
}
void FBBBMaodieTestInitializer::Initialize(ABBBMaodieTest &Actor)
{
    auto &State = Actor.RuntimeData.Behavior.State;
    const auto &Config = Actor.Config;
    State.Home = Actor.GetActorLocation();
    Actor.GetCharacterMovement()->MaxWalkSpeed = FMath::Max(1.0f, Config.MoveSpeed);
    const float Times[] = {Config.WanderRadius, Config.MoveSpeed, Config.PauseSeconds, Config.DetectionRadius,
        Config.ObserveSeconds, Config.ThrowFrequency, Config.HissDuration, Config.LoseInterestDelay, Config.TurnSpeed};
    bool bValid = Config.ThrowLimit >= 0 && !Config.DropPool.IsEmpty() && Config.HissSound
        && Config.WalkAnimation && Config.IdleAnimation
        && FMath::IsFinite(Config.IntervalJitter) && Config.IntervalJitter >= 0.0f && Config.IntervalJitter <= 0.9f
        && FMath::IsFinite(Config.ThrowSpeedMin) && FMath::IsFinite(Config.ThrowSpeedMax)
        && Config.ThrowSpeedMin >= 0.0f && Config.ThrowSpeedMax >= Config.ThrowSpeedMin
        && FMath::IsFinite(Config.ThrowUpSpeedMin) && FMath::IsFinite(Config.ThrowUpSpeedMax)
        && Config.ThrowUpSpeedMin > 0.0f && Config.ThrowUpSpeedMax >= Config.ThrowUpSpeedMin
        && FMath::IsFinite(Config.ThrowHeightMin) && FMath::IsFinite(Config.ThrowHeightMax)
        && Config.ThrowHeightMin >= 0.0f && Config.ThrowHeightMax >= Config.ThrowHeightMin;
    for (const float Value : Times)
    {
        bValid &= FMath::IsFinite(Value) && Value > 0.0f;
    }
    for (const auto &Definition : Config.DropPool)
    {
        bValid &= IsValid(Definition) && !Definition->ItemId.IsNone();
    }
    State.bEnabled = bValid;
    if (!bValid)
    {
        UE_LOG(LogTemp, Error, TEXT("耄耋测试配置无效 请配置物品池 音效 动画和有限正数行为参数 %s"), *Actor.GetPathName());
    }
}
