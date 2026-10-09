#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/Core/Initialization/BBBMeleeInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Animation/BBBMeleeAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterEquipmentActionLifecycleLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterEquipmentContactWindowLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
bool FBBBMeleeInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMeleeEquipment &>(BaseEquipment);
    const auto *Definition = Cast<UBBBMeleeDefinition>(Equipment.GetDefinition());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    if (!ensureMsgf(Definition && Definition->AttackMontage && Mesh
        && Definition->EquipmentType == EBBBEquipmentType::Melee
        && Definition->Damage >= 0.0f && Definition->TraceRadius > 0.0f && Definition->AttackInterval > 0.0f
        && Cast<UBBBMeleeAnimInstance>(Equipment.GetEquipmentAnimationInstance()), TEXT("近战装备缺少有效配置与动画实例")))
    {
        return false;
    }
    if (!ensureMsgf(Mesh->DoesSocketExist(Definition->TraceStartSocket) && Mesh->DoesSocketExist(Definition->TraceEndSocket),
        TEXT("近战扫掠插槽不存在")))
    {
        return false;
    }
    if (!ensureMsgf(Definition->AttackMontage->SlotAnimTracks.Num() == 1,
        TEXT("近战攻击只能包含一个 UpperBody 轨道")))
    {
        return false;
    }
    bool bHasLifecycle = false;
    bool bHasContact = false;
    for (const auto &Notify : Definition->AttackMontage->Notifies)
    {
        bHasLifecycle |= Notify.NotifyStateClass && Notify.NotifyStateClass->IsA<UBBBCharacterEquipmentActionLifecycleLogicAnimNotifyState>();
        bHasContact |= Notify.NotifyStateClass && Notify.NotifyStateClass->IsA<UBBBCharacterEquipmentContactWindowLogicAnimNotifyState>();
    }
    if (!ensureMsgf(bHasLifecycle && bHasContact, TEXT("近战攻击必须配置动作生命周期与命中窗口通知")))
    {
        return false;
    }
    for (const auto &Track : Definition->AttackMontage->SlotAnimTracks)
    {
        if (!ensureMsgf(ABBBCharacter::ClassifyMontageSlot(Track.SlotName) == EBBBCharacterMontageSlot::UpperBody,
            TEXT("近战攻击必须配置角色 UpperBody 蒙太奇")))
        {
            return false;
        }
    }
    UE_LOG(LogTemp, Log, TEXT("[BBBMelee] Initialized Equipment=%s Damage=%.1f Interval=%.3f"),
        *Definition->ItemId.ToString(), Definition->Damage, Definition->AttackInterval);
    return true;
}
