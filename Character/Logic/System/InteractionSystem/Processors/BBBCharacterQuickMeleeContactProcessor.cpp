#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterQuickMeleeContactProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/HitReaction/FBBBMonsterHitReactionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBodyPartDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

void FBBBCharacterQuickMeleeContactProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Interaction.InteractionUseState;
    auto &Inputs = Context.RuntimeData.Interaction.InteractionInputState;
    if (Inputs.MeleeBeginTokens.Contains(State.UseSerial))
    {
        State.bMeleeContact = true;
        State.bHasMeleePose = false;
    }
    if (Inputs.MeleeEndTokens.Contains(State.UseSerial))
    {
        State.bMeleeContact = false;
    }
    Inputs.MeleeBeginTokens.Reset();
    Inputs.MeleeEndTokens.Reset();
    if (Context.bIsMirror || !State.bBusy || !State.Fact.bQuickMelee || !State.bMeleeContact ||
        !Context.Character.IsItemUsable() || !Context.Character.GetWorld())
    {
        return;
    }
    auto &World = *Context.Character.GetWorld();
    const auto &Config = Context.Character.GetCharacterConfig();
    const FVector Base = Context.CharacterMesh.GetSocketLocation(TEXT("hand_r"));
    const FVector Tip = Base + Context.Character.GetActorForwardVector() * Config.QuickMeleeReach;
    const FVector PreviousBase = State.bHasMeleePose ? State.PreviousMeleeBase : Base;
    const FVector PreviousTip = State.bHasMeleePose ? State.PreviousMeleeTip : Tip;
    State.PreviousMeleeBase = Base;
    State.PreviousMeleeTip = Tip;
    State.bHasMeleePose = true;
    auto *Mass = World.GetSubsystem<UBBBMassSubsystem>();
    FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMelee), false);
    Params.AddIgnoredActor(&Context.Character);
    Params.AddIgnoredActor(Context.Character.GetHeldItem());
    const float Length = FVector::Distance(Base, Tip);
    const int32 Samples = FMath::Clamp(FMath::CeilToInt(Length / (Config.QuickMeleeRadius * 1.5f)), 2, 32);
    for (int32 Index = -1; Index <= Samples; ++Index)
    {
        const float Fraction = float(FMath::Max(Index, 0)) / Samples;
        const FVector Start = Index < 0 ? Base : FMath::Lerp(PreviousBase, PreviousTip, Fraction);
        const FVector End = Index < 0 ? Tip : FMath::Lerp(Base, Tip, Fraction);
        FHitResult Hit;
        const bool bWorldHit = World.SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility,
                                                          FCollisionShape::MakeSphere(Config.QuickMeleeRadius), Params);
        FMassEntityHandle Entity;
        float HitTime = 1.0f;
        FVector Position = FVector::ZeroVector;
        FVector Normal = FVector::ZeroVector;
        EPhysicalSurface Surface = SurfaceType_Default;
        uint8 Part = 0;
        const bool bEntityHit = Mass && Mass->TraceEntities(Start, End, Config.QuickMeleeRadius, State.MeleeEntities,
                                                            Entity, HitTime, Position, Normal, Surface, Part);
        const FVector Direction = (End - Start).GetSafeNormal(SMALL_NUMBER, Context.Character.GetActorForwardVector());
        if (bEntityHit && (!bWorldHit || HitTime < Hit.Time))
        {
            auto *Player = Context.Character.GetPlayerState();
            if (!ensureMsgf(Player && Player->GetPlayerId() >= 0, TEXT("近战伤害缺少稳定玩家身份")))
            {
                continue;
            }
            FBBBMonsterDamageLocalControlPacket Packet;
            if (!Mass->QueryDamage(Entity, Packet.Contributions))
            {
                continue;
            }
            const auto *Existing = Packet.Contributions.FindByPredicate(
                [Player](const auto &Value)
                {
                    return Value.PlayerId == Player->GetPlayerId();
                });
            FBBBMonsterDamageContribution Value = Existing ? *Existing : FBBBMonsterDamageContribution{};
            Value.PlayerId = Player->GetPlayerId();
            Value.LastHitRegion = static_cast<EBBBMonsterHitRegion>(Part);
            FBBBMonsterBodyPartDefinition PartDefinition;
            if (!Mass->QueryMonsterBodyPart(Entity, Part, PartDefinition))
            {
                continue;
            }
            Value.Parts.Add(Value.LastHitRegion, Config.QuickMeleeDamage * (1.0 - PartDefinition.Durability) +
                                                     Config.QuickMeleeDurableDamage * PartDefinition.Durability);
            const auto *GameState = World.GetGameState();
            Value.LastHitTime = GameState ? GameState->GetServerWorldTimeSeconds() : World.GetTimeSeconds();
            Packet.Include(Value);
            if (Mass->SubmitInput(Entity, MoveTemp(Packet)))
            {
                State.MeleeEntities.Add(Entity);
                FBBBMonsterHitReactionLocalControlPacket Reaction;
                Reaction.Region = static_cast<EBBBMonsterHitRegion>(Part);
                Reaction.Position = Position;
                Reaction.Normal = Normal;
                Reaction.Direction = Direction;
                Mass->SubmitInput(Entity, MoveTemp(Reaction));
                UE_LOG(LogTemp, Log, TEXT("[BBBMelee] Mass hit Attack=%d Entity=%d Damage=%.1f"), State.UseSerial,
                       Entity.Index, Config.QuickMeleeDamage);
            }
            continue;
        }
        AActor *Actor = bWorldHit ? Hit.GetActor() : nullptr;
        if (IsValid(Actor) && Actor->CanBeDamaged() && !State.MeleeActors.Contains(Actor))
        {
            State.MeleeActors.Add(Actor);
            UGameplayStatics::ApplyPointDamage(Actor, Config.QuickMeleeDamage, Direction, Hit,
                                               Context.Character.GetController(), &Context.Character, nullptr);
            UE_LOG(LogTemp, Log, TEXT("[BBBMelee] Actor hit Attack=%d Target=%s Damage=%.1f"), State.UseSerial,
                   *Actor->GetName(), Config.QuickMeleeDamage);
        }
    }
}
