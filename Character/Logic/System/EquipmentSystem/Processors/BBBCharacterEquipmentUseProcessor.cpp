#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentUseProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentUnequipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentUnequipAuthorityFactPacket.h"

void FBBBCharacterEquipmentUseProcessor::Update(FBBBCharacterEquipmentUpdateContext &Context) const
{
    auto &Domain = Context.RuntimeData.Equipment;
    auto &State = Domain.EquipmentUseState;
    auto &Input = Domain.EquipmentUseInputState;
    const auto &Binding = Domain.ReadEquipmentSelectionState();
    ABBBEquipment *Equipment = Binding.ActiveMainHandInstance;
    const bool bSameBinding = State.Generation == Binding.ActiveGeneration;
    bool bUsable = State.bUsable;
    bool bHasResult = !Context.bIsMirror;
    const bool bInitialResult = !State.bInitialized;
    const bool bPreviouslyUsable = State.bUsable && bSameBinding;
    uint64 Revision = State.Revision;
    if (!Context.bIsMirror)
    {
        const auto &Traversal = Context.RuntimeData.Traversal.ReadTraversalState();
        // 装备恢复跟随移动交接 不等待已退出攀爬的姿势尾段完全消失
        bUsable = Context.RuntimeData.Life.ReadLifeState().bActionsAllowed &&
                  (Traversal.Action == EBBBTraversalAction::None || Traversal.bAnimationReleased);
        if (!bSameBinding || bInitialResult || State.bUsable != bUsable)
        {
            ++Revision;
        }
    }
    if (Context.bIsMirror)
    {
        for (int32 Index = Input.Generations.Num() - 1; Index >= 0; --Index)
        {
            if (Input.Generations[Index] == Binding.ActiveGeneration && Input.Revisions[Index] > Revision)
            {
                bUsable = Input.Usable[Index];
                Revision = Input.Revisions[Index];
                bHasResult = true;
            }
            if (Input.Generations[Index] <= Binding.ActiveGeneration)
            {
                Input.Generations.RemoveAt(Index);
                Input.Revisions.RemoveAt(Index);
                Input.Usable.RemoveAt(Index);
            }
        }
    }
    if (!bHasResult && !bSameBinding)
    {
        bUsable = false;
    }
    if (IsValid(Equipment) && !bUsable && bPreviouslyUsable)
    {
        // 清理仍交给装备自身 本帧保持更新直到装备消费解除使用请求
        Equipment->SetActorTickEnabled(true);
        if (Context.bIsMirror)
        {
            Equipment->SubmitInput(FBBBEquipmentUnequipAuthorityFactPacket{});
        }
        if (!Context.bIsMirror)
        {
            Equipment->SubmitInput(FBBBEquipmentUnequipLocalControlPacket{});
        }
    }
    State.Generation = Binding.ActiveGeneration;
    State.bUsable = bUsable;
    if (bHasResult)
    {
        State.Revision = Revision;
        State.bInitialized = true;
    }
    if (!IsValid(Equipment))
    {
        return;
    }
    if (bHasResult && bUsable)
    {
        Equipment->SetActorTickEnabled(true);
        // 初始镜像快照只建立现状 后续真正恢复才播放一次装备表现
        if (!bPreviouslyUsable && (!Context.bIsMirror || (!bInitialResult && bSameBinding)))
        {
            if (Context.bIsMirror)
            {
                Equipment->SubmitInput(FBBBEquipmentEquipAuthorityFactPacket{});
            }
            if (!Context.bIsMirror)
            {
                Equipment->SubmitInput(FBBBEquipmentEquipLocalControlPacket{});
            }
        }
    }
    if (!bUsable && !bPreviouslyUsable)
    {
        Equipment->SetActorTickEnabled(false);
    }
    Equipment->SetActorHiddenInGame(!bUsable);
}
