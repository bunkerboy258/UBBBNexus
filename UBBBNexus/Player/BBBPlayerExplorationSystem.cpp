#include "BBBWork/UBBBNexus/Player/BBBPlayerExplorationSystem.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "HAL/PlatformTime.h"

/** 本机探索事实的唯一写入者 */
class FBBBPlayerExplorationProcessor final
{
public:
    /** @param System 所属探索模型 @param Controller 本地观察者 @return 无 */
    static void Update(UBBBPlayerExplorationSystem &System, APlayerController &Controller)
    {
        auto *World = Controller.GetWorld();
        if (!Controller.IsLocalController() || !World || !World->IsGameWorld())
        {
            return;
        }
        auto &State = System.RuntimeData.Exploration.State;
        if (State.World.Get() != World)
        {
            State.World = World;
            State.DiscoveredMask = 0;
            State.NextScanAt = 0.0;
            State.CompletedAt = -1.0;
        }
        const double Now = FPlatformTime::Seconds();
        const auto *Character = Cast<ABBBCharacter>(Controller.GetPawn());
        if (!Character || Character->GetLifePhase() != EBBBCharacterLifePhase::Alive
            || Now < State.NextScanAt || State.CompletedAt >= 0.0)
        {
            return;
        }
        State.NextScanAt = Now + 0.2;
        const FVector Eye = Character->GetPawnViewLocation();
        for (TActorIterator<ABBBMaodieTest> It(World); It; ++It)
        {
            const auto &Target = **It;
            const auto &Config = Target.GetTestConfig();
            const int32 Index = Config.DiscoveryIndex;
            if (Index < 1 || Index > UBBBPlayerExplorationSystem::TargetCount || Target.IsHidden()
                || Target.IsActorBeingDestroyed() || !FMath::IsFinite(Config.DetectionRadius)
                || Config.DetectionRadius <= 0.0f)
            {
                continue;
            }
            const uint16 Bit = uint16(1u << (Index - 1));
            if ((State.DiscoveredMask & Bit) != 0
                || FVector::DistSquared(Character->GetActorLocation(), Target.GetActorLocation()) > FMath::Square(Config.DetectionRadius))
            {
                continue;
            }
            FHitResult Hit;
            FCollisionQueryParams Query(SCENE_QUERY_STAT(MaodieDiscovery), true, Character);
            Query.AddIgnoredActor(&Target);
            const FVector TargetEye = Target.GetActorLocation() + FVector(0.0f, 0.0f, 20.0f);
            if (World->LineTraceSingleByChannel(Hit, Eye, TargetEye, ECC_Visibility, Query))
            {
                continue;
            }
            State.DiscoveredMask |= Bit;
        }
        if (State.DiscoveredMask == uint16((1u << UBBBPlayerExplorationSystem::TargetCount) - 1u))
        {
            State.CompletedAt = Now;
        }
    }
};

void UBBBPlayerExplorationSystem::Update(APlayerController &Controller)
{
    FBBBPlayerExplorationProcessor::Update(*this, Controller);
}

int32 UBBBPlayerExplorationSystem::GetDiscoveredCount() const
{
    return FMath::CountBits(uint32(RuntimeData.Exploration.ReadPlayerExplorationState().DiscoveredMask));
}

double UBBBPlayerExplorationSystem::GetCompletionTime() const
{
    return RuntimeData.Exploration.ReadPlayerExplorationState().CompletedAt;
}
