#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Combat/BBBMonsterCombatProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "GameFramework/Pawn.h"
#include "MassActorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

UBBBMonsterCombatProcessor::UBBBMonsterCombatProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Collision;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterAvoidanceProcessor::StaticClass()->GetFName());
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterLocomotionProcessor::StaticClass()->GetFName());
}

void UBBBMonsterCombatProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddRequirement<FBBBMonsterCombatFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterCombatProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    // 获取攻击判定使用的世界
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster combat requires a valid world")))
    {
        return;
    }

    // 使用同一时间值处理当前批次
    const float WorldTime = World->GetTimeSeconds();

    MonsterQuery.ForEachEntityChunk(Context, [WorldTime, World](FMassExecutionContext& ChunkContext)
    {
        // 只处理已经进入攻击状态且冷却完成的实体
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const auto Actors = ChunkContext.GetFragmentView<FMassActorFragment>();
        const auto States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Grounds = ChunkContext.GetFragmentView<FBBBMonsterGroundFragment>();
        auto Combats = ChunkContext.GetMutableFragmentView<FBBBMonsterCombatFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FBBBMonsterCombatFragment& Combat = Combats[Index];

            if (States[Index].State != EBBBMonsterBehavior::Attack)
            {
                continue;
            }

            if (!Grounds[Index].bGrounded)
            {
                Combat.AttackTarget.Reset();
                Combat.bHitAttempted = true;
                Combat.bAttackFinished = true;
                continue;
            }

            const float Elapsed = FMath::Max(WorldTime - States[Index].StateEnteredTime, 0.0f);
            Combat.bAttackFinished = Elapsed >= Combat.AttackWindup + Combat.AttackRecovery;

            if (Combat.bHitAttempted || Elapsed < Combat.AttackWindup)
            {
                continue;
            }

            // 先标记已判定再调用外部伤害回调 未命中也消耗本轮机会
            Combat.bHitAttempted = true;
            AActor* PlayerPawn = Combat.AttackTarget.Get();

            // 没有玩家时不执行任何攻击判定
            if (!IsValid(PlayerPawn) || !PlayerPawn->CanBeDamaged())
            {
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Attack miss AttackId=%u Reason=InvalidTarget"), Combat.AttackId);
                continue;
            }

            const FVector MonsterLocation = Transforms[Index].GetTransform().GetLocation();

            if (FVector::DistSquared(MonsterLocation, PlayerPawn->GetActorLocation()) > FMath::Square(Combat.AttackRange))
            {
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Attack miss AttackId=%u Reason=OutOfRange"), Combat.AttackId);
                continue;
            }

            AActor* DamageCauser = Actors.IsEmpty() ? nullptr : const_cast<AActor*>(Actors[Index].Get());
            APawn* CauserPawn = Cast<APawn>(DamageCauser);
            AController* Instigator = CauserPawn != nullptr ? CauserPawn->GetController() : nullptr;

            const FVector ToTarget = PlayerPawn->GetActorLocation() - MonsterLocation;
            const FVector HorizontalDirection = ToTarget.GetSafeNormal2D();
            const FVector Facing = Transforms[Index].GetTransform().GetRotation().GetForwardVector().GetSafeNormal2D();
            if (!HorizontalDirection.IsNearlyZero() && FVector::DotProduct(Facing, HorizontalDirection) < 0.5f)
            {
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Attack miss AttackId=%u Reason=OutsideSwing"), Combat.AttackId);
                continue;
            }

            UPrimitiveComponent* TargetBody = Cast<UPrimitiveComponent>(PlayerPawn->GetRootComponent());
            FVector Contact = PlayerPawn->GetActorLocation();
            if (TargetBody)
            {
                FVector Closest;
                if (TargetBody->GetClosestPointOnCollision(MonsterLocation, Closest) >= 0.0f)
                {
                    Contact = Closest;
                }
            }
            FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterAttack), false);
            Params.AddIgnoredActor(PlayerPawn);
            if (DamageCauser)
            {
                Params.AddIgnoredActor(DamageCauser);
            }
            FHitResult Obstruction;
            if (World->LineTraceSingleByChannel(Obstruction, MonsterLocation, Contact, ECC_Pawn, Params))
            {
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Attack miss AttackId=%u Reason=Obstructed"), Combat.AttackId);
                continue;
            }
            const FVector Direction = ToTarget.GetSafeNormal();
            FHitResult ContactHit(PlayerPawn, TargetBody, Contact, -Direction);

            // 伤害通过 UE 标准接口发送给玩家
            const float AppliedDamage = UGameplayStatics::ApplyPointDamage(PlayerPawn, Combat.AttackDamage,
                Direction, ContactHit, Instigator, DamageCauser, nullptr);
            UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Monster attack Target=%s AttackId=%u Requested=%.1f Applied=%.1f"),
                *GetNameSafe(PlayerPawn), Combat.AttackId, Combat.AttackDamage, AppliedDamage);
        }
    });
}
