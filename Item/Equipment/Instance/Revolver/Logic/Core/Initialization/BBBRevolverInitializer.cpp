#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/Core/Initialization/BBBRevolverInitializer.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/Processors/BBBRevolverActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"

bool FBBBRevolverInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBRevolverEquipment &>(BaseEquipment);
    const UBBBRevolverDefinition *RevolverDefinition = Cast<UBBBRevolverDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(RevolverDefinition, TEXT("左轮必须配置 UBBBRevolverDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBRevolver] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *RevolverDefinition->GetPathName(), RevolverDefinition->FireInterval);

    FBBBRevolverActionProcessor::Initialize(Equipment.RuntimeData, *RevolverDefinition);
    if (RevolverDefinition->ProjectileDefinition && Equipment.GetWorld())
    {
        if (auto *Display = Equipment.GetWorld()->GetSubsystem<UBBBProjectilePresentationSubsystem>())
        {
            Display->Prepare(*RevolverDefinition->ProjectileDefinition);
        }
    }
    return true;
}
