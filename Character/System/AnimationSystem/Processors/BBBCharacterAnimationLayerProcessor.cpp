#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationLayerProcessor.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationLayerProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterAnimationConfig &AnimationConfig,
    FBBBAnimationRuntimeData &AnimationData,
    USkeletalMeshComponent &CharacterMesh) const
{
    TSubclassOf<UAnimInstance> DesiredLayerClass = AnimationConfig.DefaultAnimationLayerClass;
    UBBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();

    if (ActiveInstance)
    {
        UBBBEquipmentDefinition *Definition = ActiveInstance->GetDefinition();
        if (!ensureMsgf(
            Definition,
            TEXT("[UBBBC]Animation layer selection failed because active equipment definition is null")))
        {
            return;
        }

        if (Definition->CharacterAnimationLayerClass)
        {
            DesiredLayerClass = Definition->CharacterAnimationLayerClass;
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
