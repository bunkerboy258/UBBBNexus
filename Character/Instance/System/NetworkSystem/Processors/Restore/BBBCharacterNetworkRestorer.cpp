#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Restore/BBBCharacterNetworkRestorer.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

FBBBCharacterRestoreInput FBBBCharacterNetworkRestorer::Build(
    FBBBNetworkRuntimeData &NetworkData, UBBBEquipmentCatalog &Catalog)
{
    FBBBCharacterRestoreInput Input;
    for (const FBBBEquipmentNetworkPacket &Packet : NetworkData.RestoreEquipmentPackets())
    {
        UBBBEquipmentDefinition *Definition = Catalog.FindDefinition(Packet.EquipmentHandle);
        if (ensureMsgf(Definition, TEXT("[UBBBC]Unknown equipment restore handle %s"), *Packet.EquipmentHandle.ToString()))
        {
            Input.Equipment = Definition;
            Input.bEquipmentChanged = true;
        }
    }
    for (const FBBBEquipmentActionNetworkPacket &Packet : NetworkData.RestoreEquipmentActionPackets())
    {
        // 同一角色动作序号单调递增 同序号的换弹阶段只允许前进
        const int32 Phase = static_cast<int32>(Packet.Phase);
        if (Packet.Sequence < LastActionSequence
            || (Packet.Sequence == LastActionSequence && Phase <= LastActionPhase))
        {
            UE_LOG(LogTemp, Verbose, TEXT("[UBBBC]Ignored stale restore Sequence=%d Phase=%d"), Packet.Sequence, Phase);
            continue;
        }
        LastActionSequence = Packet.Sequence;
        LastActionPhase = Phase;
        FBBBEquipmentActionEvent &Event = Input.Actions.AddDefaulted_GetRef();
        Event.ActionType = Packet.ActionType;
        Event.EquipmentId = Packet.EquipmentId;
        Event.Sequence = Packet.Sequence;
        Event.Phase = Packet.Phase;
        Event.LoadedAmmo = Packet.LoadedAmmo;
    }
    const TOptional<FBBBAimNetworkState> Aim = NetworkData.RestoreAimState();
    if (Aim.IsSet())
    {
        FBBBAimRuntimeState State;
        State.bIsAiming = Aim->bIsAiming;
        State.AimTargetWorld = Aim->AimTargetWorld;
        Input.Aim = State;
    }
    const TOptional<FBBBLocomotionNetworkState> Gait = NetworkData.RestoreLocomotionState();
    if (Gait.IsSet())
    {
        Input.Gait = Gait->Gait;
    }
    return Input;
}
