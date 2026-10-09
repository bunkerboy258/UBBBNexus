#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeContactProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeUpdateContext.h"
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

void FBBBMeleeContactProcessor::Update(FBBBMeleeUpdateContext &Context)
{
    auto &State = Context.Data.Action.ActionState;
    if (!State.bContactOpen || !State.bAttacking || !Context.bCausal)
    {
        return;
    }
    const FVector Base = Context.Mesh.GetSocketLocation(Context.Definition.TraceStartSocket);
    const FVector Tip = Context.Mesh.GetSocketLocation(Context.Definition.TraceEndSocket);
    const FVector PreviousBase = State.bHasPreviousPose ? State.PreviousBase : Base;
    const FVector PreviousTip = State.bHasPreviousPose ? State.PreviousTip : Tip;
    State.PreviousBase = Base;
    State.PreviousTip = Tip;
    State.bHasPreviousPose = true;
    auto *Mass = Context.World.GetSubsystem<UBBBMassSubsystem>();
    FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMelee), false);
    Params.AddIgnoredActor(&Context.Character);
    Params.AddIgnoredActor(&Context.Equipment);
    const float Length = FVector::Distance(Base, Tip);
    const int32 Samples = FMath::Clamp(FMath::CeilToInt(Length / (Context.Definition.TraceRadius * 1.5f)), 2, 32);
    for (int32 Index = -1; Index <= Samples; ++Index)
    {
        const float Fraction = float(FMath::Max(Index, 0)) / Samples;
        const FVector Start = Index < 0 ? Base : FMath::Lerp(PreviousBase, PreviousTip, Fraction);
        const FVector End = Index < 0 ? Tip : FMath::Lerp(Base, Tip, Fraction);
        FHitResult Hit;
        const bool bWorldHit = Context.World.SweepSingleByChannel(Hit, Start, End, FQuat::Identity,
            Context.Definition.CollisionChannel, FCollisionShape::MakeSphere(Context.Definition.TraceRadius), Params);
        FMassEntityHandle Entity;
        float HitTime = 1.0f;
        FVector Position = FVector::ZeroVector;
        FVector Normal = FVector::ZeroVector;
        EPhysicalSurface Surface = SurfaceType_Default;
        uint8 Part = 0;
        const bool bEntityHit = Mass && Mass->TraceEntities(Start, End, Context.Definition.TraceRadius,
            State.HitEntities, Entity, HitTime, Position, Normal, Surface, Part);
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
            const auto *Existing = Packet.Contributions.FindByPredicate([Player](const auto &Value)
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
            Value.Parts.Add(Value.LastHitRegion, Context.Definition.Damage * (1.0 - PartDefinition.Durability)
                + Context.Definition.DurableDamage * PartDefinition.Durability);
            const auto *GameState = Context.World.GetGameState();
            Value.LastHitTime = GameState ? GameState->GetServerWorldTimeSeconds() : Context.World.GetTimeSeconds();
            Packet.Include(Value);
            if (Mass->SubmitInput(Entity, MoveTemp(Packet)))
            {
                State.HitEntities.Add(Entity);
                ++State.HitCount;
                FBBBMonsterHitReactionLocalControlPacket Reaction;
                Reaction.Region = static_cast<EBBBMonsterHitRegion>(Part);
                Reaction.Position = Position;
                Reaction.Normal = Normal;
                Reaction.Direction = Direction;
                Mass->SubmitInput(Entity, MoveTemp(Reaction));
                UE_LOG(LogTemp, Log, TEXT("[BBBMelee] Mass hit Attack=%d Entity=%d Damage=%.1f"),
                    State.AttackSequence, Entity.Index, Context.Definition.Damage);
            }
            continue;
        }
        AActor *Actor = bWorldHit ? Hit.GetActor() : nullptr;
        if (IsValid(Actor) && Actor->CanBeDamaged() && !State.HitActors.Contains(Actor))
        {
            State.HitActors.Add(Actor);
            UGameplayStatics::ApplyPointDamage(Actor, Context.Definition.Damage, Direction, Hit,
                Context.Character.GetController(), &Context.Equipment, nullptr);
            ++State.HitCount;
            UE_LOG(LogTemp, Log, TEXT("[BBBMelee] Actor hit Attack=%d Target=%s Damage=%.1f"),
                State.AttackSequence, *Actor->GetName(), Context.Definition.Damage);
        }
    }
}
