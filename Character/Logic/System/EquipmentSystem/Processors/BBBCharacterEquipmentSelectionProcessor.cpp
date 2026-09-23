#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentCommandState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentInventoryState.h"
#include "BBBWork/UBBBNexus/Equipment/Template/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Animation/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(
    FBBBCharacterEquipmentUpdateContext &Context) const
{
    ABBBCharacter &Character = Context.Character;
    USkeletalMeshComponent &CharacterMesh = Context.CharacterMesh;
    FBBBCharacterEquipmentCommandState &EquipmentCommands = Context.CommandState;
    FBBBCharacterEquipmentSelectionState &EquipmentState = Context.SelectionState;
    FBBBCharacterEquipmentInventoryState &InventoryState = Context.InventoryState;

    // 先处理网络恢复的装备实例
    bool bRestoringEquipment = false;
    TSubclassOf<ABBBEquipment> StateClass = EquipmentCommands.PendingEquipmentClass;
    EquipmentCommands.PendingEquipmentClass = nullptr;
    if (StateClass)
    {
        bool bHasBackpackSlot = false;
        for (const FBBBCharacterItem &Item : InventoryState.BackpackSlots)
        {
            if (!IsValid(Item.ItemActor.Get())
                || Item.ItemActor.Get() == EquipmentState.ActiveMainHandInstance)
            {
                bHasBackpackSlot = true;
                break;
            }
        }

        if (!ensureMsgf(
            bHasBackpackSlot,
            TEXT("角色 %s 的背包已满 无法恢复装备 %s"),
            *Character.GetName(),
            *StateClass->GetName()))
        {
            return;
        }

        ABBBEquipment *StateInstance = FBBBCharacterEquipmentLifecycleProcessor::Create(
            Character,
            StateClass,
            Context.bIsMirror);
        if (!StateInstance)
        {
            return;
        }

        // 恢复实例作为新的目标装备等待后续附着
        EquipmentState.DesiredMainHandInstance = StateInstance;
        bRestoringEquipment = true;
    }

    if (EquipmentState.ActiveMainHandInstance == EquipmentState.DesiredMainHandInstance)
    {
        return;
    }

    // 目标变化时先收束当前主手装备
    if (EquipmentState.ActiveMainHandInstance)
    {
        if (bRestoringEquipment)
        {
            ABBBEquipment *PreviousInstance = EquipmentState.ActiveMainHandInstance;
            FBBBCharacterEquipmentLifecycleProcessor::Destroy(&CharacterMesh, *EquipmentState.ActiveMainHandInstance);

            for (FBBBCharacterItem &Item : InventoryState.BackpackSlots)
            {
                if (Item.ItemActor.Get() == PreviousInstance || !IsValid(Item.ItemActor.Get()))
                {
                    Item.ItemActor = nullptr;
                }
            }

            for (FBBBCharacterItem &Item : InventoryState.ItemBarSlots)
            {
                if (Item.ItemActor.Get() == PreviousInstance || !IsValid(Item.ItemActor.Get()))
                {
                    Item.ItemActor = nullptr;
                }
            }
        }
        if (!bRestoringEquipment)
        {
            FBBBCharacterEquipmentLifecycleProcessor::Detach(&CharacterMesh, *EquipmentState.ActiveMainHandInstance);
        }
    }

    // 清除旧装备的换弹序号并切换当前实例引用
    EquipmentState.ActiveMainHandInstance = EquipmentState.DesiredMainHandInstance;
    ABBBEquipment *DesiredInstance = EquipmentState.ActiveMainHandInstance;
    EquipmentState.ActiveEquipmentId = DesiredInstance ? DesiredInstance->GetEquipmentId() : NAME_None;
    if (!DesiredInstance)
    {
        return;
    }

    // 附着失败时清理目标装备并回到未装备状态
    if (!FBBBCharacterEquipmentLifecycleProcessor::Attach(
        CharacterMesh,
        Context.RightHandWeaponSocketName,
        *DesiredInstance))
    {
        if (bRestoringEquipment)
        {
            FBBBCharacterEquipmentLifecycleProcessor::Destroy(&CharacterMesh, *DesiredInstance);
            EquipmentState.DesiredMainHandInstance = nullptr;
        }
        if (!bRestoringEquipment)
        {
            FBBBCharacterEquipmentLifecycleProcessor::Detach(&CharacterMesh, *DesiredInstance);
        }

        EquipmentState.ActiveMainHandInstance = nullptr;
        EquipmentState.ActiveEquipmentId = NAME_None;
        return;
    }

    if (bRestoringEquipment)
    {
        bool bStoredInBackpack = false;
        for (FBBBCharacterItem &Item : InventoryState.BackpackSlots)
        {
            if (!IsValid(Item.ItemActor.Get()))
            {
                Item.ItemActor = DesiredInstance;
                bStoredInBackpack = true;
                break;
            }
        }

        ensureMsgf(
            bStoredInBackpack,
            TEXT("角色 %s 创建装备 %s 后未找到可用背包槽"),
            *Character.GetName(),
            *DesiredInstance->GetEquipmentId().ToString());

        for (FBBBCharacterItem &Item : InventoryState.ItemBarSlots)
        {
            if (!IsValid(Item.ItemActor.Get()))
            {
                Item.ItemActor = DesiredInstance;
                break;
            }
        }

        return;
    }

    DesiredInstance->SubmitEquipInput(
        EquipmentState.NextActionSequence++,
        Context.bIsMirror);
}
