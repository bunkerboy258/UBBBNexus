#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "HAL/IConsoleManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "MassEntitySubsystem.h"
#include "MassEntityQuery.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "GameFramework/GameStateBase.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"

namespace
{
    FGuid TestedMonster;
    float ExpectedHealth = 0.0f;
    int32 TestWorlds = 0;
    TArray<int32> TestedPlayers;

    FMassEntityHandle FindTestMonster(UWorld& World)
    {
        for (TActorIterator<ABBBMassNetworkActor> It(&World); It; ++It)
        {
            return It->FindEntity(TestedMonster);
        }
        return {};
    }

    /** 在真实三世界 PIE 内验证两个客机同轮提交 可靠 RPC 主机合并与本机死亡 */
    FAutoConsoleCommand TestDamageSync(
        TEXT("bbb.mass.TestDamageSync"),
        TEXT("三世界 PIE 伤害验证 start check kill death 不修改关卡资产"),
        FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            const FString Stage = Args.IsEmpty() ? TEXT("start") : Args[0];
            if (Stage == TEXT("start"))
            {
                TestedMonster.Invalidate();
                TestedPlayers.Reset();
                TestWorlds = 0;
                for (const FWorldContext& Entry : GEngine->GetWorldContexts())
                {
                    UWorld* World = Entry.World();
                    if (!World || World->WorldType != EWorldType::PIE || World->GetNetMode() != NM_ListenServer)
                    {
                        continue;
                    }
                    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
                    FMassEntityQuery Query(Manager.AsShared());
                    Query.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
                    Query.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
                    Query.AddRequirement<FBBBMonsterDamageFragment>(EMassFragmentAccess::ReadOnly);
                    FMassExecutionContext Context(Manager, 0.0f);
                    Query.ForEachEntityChunk(Context, [](FMassExecutionContext& Chunk)
                    {
                        const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
                        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
                        const auto Damage = Chunk.GetFragmentView<FBBBMonsterDamageFragment>();
                        for (int32 Index = 0; Index < Chunk.GetNumEntities() && !TestedMonster.IsValid(); ++Index)
                        {
                            if (Network[Index].InstanceId.IsValid() && Health[Index].MaxHealth > 40.0f
                                && Damage[Index].Contributions.IsEmpty())
                            {
                                TestedMonster = Network[Index].InstanceId;
                                ExpectedHealth = Health[Index].MaxHealth - 40.0f * Network[Index].Definition->LeftLegPart.MainTransfer;
                            }
                        }
                    });
                }
                if (!TestedMonster.IsValid())
                {
                    UE_LOG(LogTemp, Error, TEXT("[BBBMassDamageCheck] FAIL no untouched host monster"));
                    return;
                }
            }

            if (Stage != TEXT("start") && (!TestedMonster.IsValid() || TestedPlayers.Num() != 2))
            {
                UE_LOG(LogTemp, Error, TEXT("[BBBMassDamageCheck] FAIL run start with two clients first"));
                return;
            }
            bool bValid = TestedMonster.IsValid();
            int32 Worlds = 0;
            for (const FWorldContext& Entry : GEngine->GetWorldContexts())
            {
                UWorld* World = Entry.World();
                if (!World || World->WorldType != EWorldType::PIE)
                {
                    continue;
                }
                ++Worlds;
                auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
                const FMassEntityHandle Entity = FindTestMonster(*World);
                if (!Manager.IsEntityValid(Entity))
                {
                    bValid = false;
                    UE_LOG(LogTemp, Warning, TEXT("[BBBMassDamageCheck] World=%s target absent"), *World->GetPathName());
                    continue;
                }

                if ((Stage == TEXT("start") || Stage == TEXT("kill")) && World->GetNetMode() == NM_Client)
                {
                    APlayerController* Controller = World->GetFirstPlayerController();
                    const APlayerState* Player = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
                    FBBBMonsterDamageLocalControlPacket Packet;
                    UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>();
                    if (!Player || !Mass->QueryDamage(Entity, Packet.Contributions))
                    {
                        bValid = false;
                        continue;
                    }
                    if (Stage == TEXT("start"))
                    {
                        TestedPlayers.AddUnique(Player->GetPlayerId());
                        FBBBMonsterDamageContribution Contribution;
                        Contribution.PlayerId = Player->GetPlayerId();
                        Contribution.Parts.LeftLeg = 20.0;
                        Contribution.LastHitRegion = EBBBMonsterHitRegion::LeftLeg;
                        Contribution.LastHitTime = World->GetGameState()->GetServerWorldTimeSeconds();
                        Packet.Include(Contribution);
                        Mass->SubmitInput(Entity, MoveTemp(Packet));
                    }
                    else if (Player->GetPlayerId() == TestedPlayers[0])
                    {
                        FBBBMonsterDamageContribution Contribution = Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity).Contributions.FindChecked(Player->GetPlayerId());
                        Contribution.Parts.Torso = Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity).MaxHealth + 20.0;
                        Contribution.LastHitRegion = EBBBMonsterHitRegion::Torso;
                        Contribution.LastHitTime = World->GetGameState()->GetServerWorldTimeSeconds();
                        Packet.Include(Contribution);
                        Mass->SubmitInput(Entity, MoveTemp(Packet));
                    }
                    continue;
                }

                if (Stage == TEXT("check") || Stage == TEXT("death"))
                {
                    const auto& Health = Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity);
                    const auto& Damage = Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity);
                    const auto& Network = Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity);
                    const auto& Behavior = Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(Entity);
                    const auto& Mobility = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(Entity);
                    const auto* A = TestedPlayers.Num() == 2 ? Damage.Contributions.Find(TestedPlayers[0]) : nullptr;
                    const auto* B = TestedPlayers.Num() == 2 ? Damage.Contributions.Find(TestedPlayers[1]) : nullptr;
                    const bool bDeath = Stage == TEXT("death");
                    const bool bWorldValid = A && B && B->Parts.Sum() == 20.0 && A->Parts.LeftLeg == 20.0 && B->Parts.LeftLeg == 20.0
                        && Mobility.bCrawling && Network.bDamageSubmitted
                        && (bDeath ? Health.CurrentHealth == 0.0f && Behavior.State == EBBBMonsterBehavior::Dead
                            : A->Parts.Sum() == 20.0 && FMath::IsNearlyEqual(Health.CurrentHealth, ExpectedHealth));
                    bValid &= bWorldValid;
                    UE_LOG(LogTemp, Display, TEXT("[BBBMassDamageCheck] Stage=%s World=%s Mode=%d A=%.1f B=%.1f Health=%.1f Dead=%d Submitted=%d Result=%s"),
                        *Stage, *World->GetPathName(), int32(World->GetNetMode()), A ? A->Parts.Sum() : -1.0, B ? B->Parts.Sum() : -1.0,
                        Health.CurrentHealth, Behavior.State == EBBBMonsterBehavior::Dead, Network.bDamageSubmitted,
                        bWorldValid ? TEXT("PASS") : TEXT("FAIL"));
                }
            }
            if (Stage == TEXT("start"))
            {
                TestWorlds = Worlds;
                bValid &= TestedPlayers.Num() == 2 && Worlds == 3;
            }
            else
            {
                bValid &= Worlds == TestWorlds && TestedPlayers.Num() == 2;
            }
            UE_LOG(LogTemp, Display, TEXT("[BBBMassDamageCheck] Stage=%s Result=%s Worlds=%d Players=%d Instance=%s"),
                *Stage, bValid ? TEXT("PASS") : TEXT("FAIL"), Worlds, TestedPlayers.Num(), *TestedMonster.ToString());
        }));
}

#endif
