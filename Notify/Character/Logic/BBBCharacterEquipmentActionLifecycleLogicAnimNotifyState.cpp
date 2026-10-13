#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterEquipmentActionLifecycleLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionEndActionLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimNotifyQueue.h"
void UBBBCharacterEquipmentActionLifecycleLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *,
    float, const FAnimNotifyEventReference &Reference)
{
    auto *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    if (!Character || Character->IsNetworkMirror() || !Reference.IsActiveContext())
    {
        return;
    }
    ABBBEquipment *Equipment = Character->GetHeldEquipment();
    const int32 Token = Reference.GetNotifyInstanceID();
    if (!Equipment || !ensureMsgf(Token >= 0, TEXT("装备动画通知缺少播放实例标识")))
    {
        return;
    }
    Recipients.FindOrAdd(MeshComp).Add(Token, Equipment);
    Character->SubmitInput(FBBBCharacterInteractionBeginActionLocalControlPacket{{Equipment}, {Token}});
}
void UBBBCharacterEquipmentActionLifecycleLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *,
    const FAnimNotifyEventReference &Reference)
{
    auto *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    auto *Bound = Recipients.Find(MeshComp);
    if (!Bound)
    {
        return;
    }
    const int32 Token = Reference.GetNotifyInstanceID();
    TWeakObjectPtr<ABBBEquipment> Equipment;
    if (!Bound->RemoveAndCopyValue(Token, Equipment))
    {
        return;
    }
    if (Bound->IsEmpty())
    {
        Recipients.Remove(MeshComp);
    }
    if (Character && !Character->IsNetworkMirror() && Equipment.IsValid())
    {
        Character->SubmitInput(FBBBCharacterInteractionEndActionLocalControlPacket{{Equipment}, {Token}});
    }
}
