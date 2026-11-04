#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/Core/Initialization/BBBSMGInitializer.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/Processors/BBBSMGActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"

bool FBBBSMGInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSMGEquipment &>(BaseEquipment);
    const UBBBSMGDefinition *SMGDefinition = Cast<UBBBSMGDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(SMGDefinition, TEXT("冲锋枪必须配置 UBBBSMGDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBSMG] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *SMGDefinition->GetPathName(), SMGDefinition->FireInterval);

    FBBBSMGActionProcessor::Initialize(Equipment.RuntimeData, *SMGDefinition);
    if (SMGDefinition->ProjectileDefinition && Equipment.GetWorld())
    {
        if (auto *Display = Equipment.GetWorld()->GetSubsystem<UBBBProjectilePresentationSubsystem>())
        {
            Display->Prepare(*SMGDefinition->ProjectileDefinition);
        }
    }
    return true;
}
