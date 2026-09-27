#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Traits/BBBMonsterTrait.h"

#include "MassEntityTemplateRegistry.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassActorSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationStateFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDeathFragment.h"

void UBBBMonsterTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    const UBBBMonsterDefinition* Settings = Definition;
    if (BuildContext.IsInspectingData())
    {
        Settings = GetDefault<UBBBMonsterDefinition>();
    }

    if (!ensureMsgf(Settings != nullptr && (BuildContext.IsInspectingData() || Settings->IsValid()), TEXT("Mass 小怪配置不完整 %s"), *GetPathNameSafe(Settings)))
    {
        return;
    }

    BuildContext.AddTag<FBBBMonsterTag>();
    BuildContext.AddTag<FMassCustomMovementTag>();
    BuildContext.AddFragment<FTransformFragment>();
    BuildContext.AddFragment<FMassVelocityFragment>();
    BuildContext.AddFragment<FMassActorFragment>();
    BuildContext.AddFragment<FBBBMonsterHealthInputFragment>();
    BuildContext.AddFragment<FBBBMonsterNetworkInputFragment>();
    BuildContext.AddFragment<FBBBMonsterDamageFragment>();
    BuildContext.AddFragment<FBBBMonsterDeathFragment>();
    BuildContext.AddFragment<FBBBMonsterBehaviorFragment>();
    BuildContext.AddFragment<FBBBMonsterTargetFragment>();
    BuildContext.AddFragment<FBBBMonsterPresentationStateFragment>();

    auto& Network = BuildContext.AddFragment_GetRef<FBBBMonsterNetworkFragment>();
    Network.Definition = Definition;
    auto& Health = BuildContext.AddFragment_GetRef<FBBBMonsterHealthFragment>();
    Health.CurrentHealth = Settings->MaxHealth;
    Health.MaxHealth = Settings->MaxHealth;
    Health.HurtDuration = Settings->HurtDuration;
    Health.DeathLifetime = Settings->DeathLifetime;
    auto& Movement = BuildContext.AddFragment_GetRef<FBBBMonsterMovementFragment>();
    Movement.MoveSpeed = Settings->MoveSpeed;
    Movement.StopRadius = Settings->StopRadius;
    auto& Perception = BuildContext.AddFragment_GetRef<FBBBMonsterPerceptionFragment>();
    Perception.SightRange = Settings->SightRange;
    auto& Avoidance = BuildContext.AddFragment_GetRef<FBBBMonsterAvoidanceFragment>();
    Avoidance.CollisionRadius = Settings->CollisionRadius;
    Avoidance.PersonalSpaceRadius = Settings->PersonalSpaceRadius;
    Avoidance.NeighborSearchRadius = Settings->NeighborSearchRadius;
    Avoidance.AvoidanceWeight = Settings->AvoidanceWeight;
    auto& Combat = BuildContext.AddFragment_GetRef<FBBBMonsterCombatFragment>();
    Combat.AttackRange = Settings->AttackRange;
    Combat.AttackDamage = Settings->AttackDamage;
    Combat.AttackCooldown = Settings->AttackCooldown;
    Combat.AttackWindup = Settings->AttackWindup;
    Combat.AttackRecovery = Settings->AttackRecovery;
    Combat.AnimationHitFraction = Settings->AnimationHitFraction;
}
