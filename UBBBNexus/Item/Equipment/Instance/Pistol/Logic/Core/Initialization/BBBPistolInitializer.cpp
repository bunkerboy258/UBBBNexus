#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/Core/Initialization/BBBPistolInitializer.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/Processors/BBBPistolActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"

bool FBBBPistolInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBPistolEquipment &>(BaseEquipment);
    const UBBBPistolDefinition *PistolDefinition = Cast<UBBBPistolDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(PistolDefinition, TEXT("手枪必须配置 UBBBPistolDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBPistol] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *PistolDefinition->GetPathName(), PistolDefinition->FireInterval);

    FBBBPistolActionProcessor::Initialize(Equipment.RuntimeData, *PistolDefinition);
    if (PistolDefinition->ProjectileDefinition && Equipment.GetWorld())
    {
        if (auto *Display = Equipment.GetWorld()->GetSubsystem<UBBBProjectilePresentationSubsystem>())
        {
            Display->Prepare(*PistolDefinition->ProjectileDefinition);
        }
    }
    return true;
}
