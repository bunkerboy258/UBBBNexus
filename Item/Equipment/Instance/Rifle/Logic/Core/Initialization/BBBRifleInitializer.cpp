#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/Core/Initialization/BBBRifleInitializer.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"

bool FBBBRifleInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBRifleEquipment &>(BaseEquipment);
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(RifleDefinition, TEXT("步枪必须配置 UBBBRifleDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBRifle] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *RifleDefinition->GetPathName(), RifleDefinition->FireInterval);

    FBBBRifleActionProcessor::Initialize(Equipment.RuntimeData, *RifleDefinition);
    if (RifleDefinition->ProjectileDefinition && Equipment.GetWorld())
    {
        if (auto *Display = Equipment.GetWorld()->GetSubsystem<UBBBProjectilePresentationSubsystem>())
        {
            Display->Prepare(*RifleDefinition->ProjectileDefinition);
        }
    }
    return true;
}
