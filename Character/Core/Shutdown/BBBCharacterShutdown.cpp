#include "BBBWork/UBBBNexus/Character/Core/Shutdown/BBBCharacterShutdown.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"

void FBBBCharacterShutdown::Shutdown(ABBBCharacter &Character)
{
    // 先停止移动完成后的更新避免收束期间访问即将销毁的装备
    Character.CharacterUpdatePipeline.LateUpdateTick.SetTickFunctionEnable(false);

    USkeletalMeshComponent *CharacterMesh = Character.GetMesh();
    const FBBBCharacterEquipmentInventoryState &InventoryState =
        Character.RuntimeData.Equipment.ReadEquipmentInventoryState();
    const FBBBCharacterEquipmentSelectionState &SelectionState =
        Character.RuntimeData.Equipment.ReadEquipmentSelectionState();
    // 使用集合合并所有装备引用避免同一实例被重复收束
    TSet<ABBBEquipment *> Instances;
    // 收集库存中的装备补齐正常持有实例
    for (const TObjectPtr<ABBBEquipment> &Instance : InventoryState.Slots)
    {
        if (Instance)
        {
            Instances.Add(Instance);
        }
    }

    // 当前装备和目标装备可能尚未写回库存因此需要单独收集
    ABBBEquipment *ActiveInstance = SelectionState.ActiveMainHandInstance;
    if (ActiveInstance)
    {
        Instances.Add(ActiveInstance);

    }

    if (ABBBEquipment *DesiredInstance = SelectionState.DesiredMainHandInstance)
    {
        Instances.Add(DesiredInstance);
    }

    // 扫描角色拥有的装备演员补齐尚未登记的实例
    if (UWorld *World = Character.GetWorld())
    {
        for (TActorIterator<ABBBEquipment> It(World); It; ++It)
        {
            if (It->GetOwner() == &Character)
            {
                Instances.Add(*It);
            }
        }
    }

    // 通过统一生命周期处理器收束仍然有效的装备
    for (ABBBEquipment *Instance : Instances)
    {
        if (IsValid(Instance))
        {
            FBBBCharacterEquipmentLifecycleProcessor::Destroy(CharacterMesh, *Instance);
        }
    }
}
