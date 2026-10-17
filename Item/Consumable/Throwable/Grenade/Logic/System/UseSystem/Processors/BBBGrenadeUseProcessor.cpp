#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeUseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/DomainData/Context/BBBGrenadeUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Config/BBBGrenadeDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyAdditiveMontageLocalControlPacket.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"

void FBBBGrenadeUseProcessor::Update(FBBBGrenadeUseUpdateContext &Context) const
{
    auto &State = Context.Data.Use.UseState;
    auto &Inputs = Context.Data.Use.UseInputState;
    if (!Inputs.bPrimary || Inputs.bFinish || State.bUsing)
    {
        return;
    }
    const auto *Equipment = Context.Character.GetHeldEquipment();
    TSubclassOf<UAnimInstance> LayerClass = Equipment ? Equipment->GetCharacterAnimationLayerClass() : nullptr;
    if (!LayerClass)
    {
        LayerClass = Context.Character.GetCharacterConfig().Animation.DefaultAnimationLayerClass;
    }
    const auto *Animation = LayerClass ? Cast<UBBBAnimInstance>(LayerClass.GetDefaultObject()) : nullptr;
    UAnimMontage *Montage = Context.Definition.GetUseMontage(Animation);
    if (!ensureMsgf(Montage && !Montage->SlotAnimTracks.IsEmpty(), TEXT("当前动画层缺少手雷投掷蒙太奇")))
    {
        Inputs.bFinish = true;
        return;
    }
    State.bUsing = true;
    if (Montage->SlotAnimTracks[0].SlotName == BBBCharacterMontageSlots::UpperBodyAdditive)
    {
        Context.Character.SubmitInput(FBBBUpperBodyAdditiveMontageLocalControlPacket{Montage});
    }
    else
    {
        Context.Character.SubmitInput(FBBBUpperBodyMontageLocalControlPacket{Montage});
    }
}
