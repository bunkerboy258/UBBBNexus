#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/Core/Initialization/BBBMinigunInitializer.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Config/BBBMinigunDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ActionSystem/Processors/BBBMinigunActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"

bool FBBBMinigunInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMinigunEquipment &>(BaseEquipment);
    const UBBBMinigunDefinition *MinigunDefinition = Cast<UBBBMinigunDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(MinigunDefinition, TEXT("转管机枪必须配置 UBBBMinigunDefinition")))
    {
        return false;
    }

    if (!ensureMsgf(FMath::IsFinite(MinigunDefinition->SpinUpSeconds) && MinigunDefinition->SpinUpSeconds >= 0.0f
        && FMath::IsFinite(MinigunDefinition->SpinDownSeconds) && MinigunDefinition->SpinDownSeconds > 0.0f
        && FMath::IsFinite(MinigunDefinition->BarrelRotationSpeedDegrees) && MinigunDefinition->BarrelRotationSpeedDegrees >= 0.0f,
        TEXT("转管机枪电机参数无效 %s"), *MinigunDefinition->GetPathName()))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBMinigun] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *MinigunDefinition->GetPathName(), MinigunDefinition->FireInterval);

    FBBBMinigunActionProcessor::Initialize(Equipment.RuntimeData, *MinigunDefinition);
    if (MinigunDefinition->ProjectileDefinition && Equipment.GetWorld())
    {
        if (auto *Display = Equipment.GetWorld()->GetSubsystem<UBBBProjectilePresentationSubsystem>())
        {
            Display->Prepare(*MinigunDefinition->ProjectileDefinition);
        }
    }
    return true;
}
