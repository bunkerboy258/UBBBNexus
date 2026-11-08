#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/Core/Initialization/BBBLMGInitializer.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Config/BBBLMGDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ActionSystem/Processors/BBBLMGActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"

bool FBBBLMGInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBLMGEquipment &>(BaseEquipment);
    const UBBBLMGDefinition *LMGDefinition = Cast<UBBBLMGDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(LMGDefinition, TEXT("轻机枪必须配置 UBBBLMGDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBLMG] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *LMGDefinition->GetPathName(), LMGDefinition->FireInterval);

    FBBBLMGActionProcessor::Initialize(Equipment.RuntimeData, *LMGDefinition);
    if (LMGDefinition->ProjectileDefinition && Equipment.GetWorld())
    {
        if (auto *Display = Equipment.GetWorld()->GetSubsystem<UBBBProjectilePresentationSubsystem>())
        {
            Display->Prepare(*LMGDefinition->ProjectileDefinition);
        }
    }
    return true;
}
