#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Processors/BBBCharacterAnimationLayerProcessor.h"

#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationLayerProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterAnimationConfig &AnimationConfig,
    FBBBAnimationRuntimeData &AnimationData,
    USkeletalMeshComponent &CharacterMesh) const
{
    TSubclassOf<UAnimInstance> DesiredLayerClass = AnimationConfig.DefaultAnimationLayerClass;
    ABBBEquipment *ActiveInstance = EquipmentState.GetActiveMainHandInstance();

    if (ActiveInstance)
    {
        const TSubclassOf<UAnimInstance> EquipmentLayerClass = ActiveInstance->GetCharacterAnimationLayerClass();
        if (EquipmentLayerClass)
        {
            DesiredLayerClass = EquipmentLayerClass;
        }
    }

    if (!ensureMsgf(
        DesiredLayerClass,
        TEXT("[UBBBC]No animation layer is configured for the character or active equipment")))
    {
        return;
    }

    if (AnimationData.LinkedAnimationLayerClass == DesiredLayerClass)
    {
        return;
    }

    CharacterMesh.LinkAnimClassLayers(DesiredLayerClass);
    AnimationData.LinkedAnimationLayerClass = DesiredLayerClass;
}
