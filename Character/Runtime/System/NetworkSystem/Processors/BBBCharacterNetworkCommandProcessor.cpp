#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBCharacterNetworkCommandProcessor.h"

#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputFrame.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/Packets/BBBCharacterCommandNetworkPackets.h"

void FBBBCharacterNetworkCommandProcessor::Update(
    FBBBCharacterInputFrame &InputFrame,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{
    const bool bHasControl = InputFrame.Movement.bActive
        || InputFrame.Aim.bActive
        || InputFrame.Fire.bActive;

    if (bHasControl)
    {
        FBBBCharacterControlNetworkPacket Packet;
        Packet.bMovementActive = InputFrame.Movement.bActive;
        Packet.bAimActive = InputFrame.Aim.bActive;
        Packet.bFire = InputFrame.Fire.bActive;

        if (InputFrame.Movement.bActive)
        {
            const FBBBCharacterMovementPacket &Movement = InputFrame.Movement.Data;
            Packet.MoveWorld = Movement.MoveWorld;
            Packet.FacingWorld = Movement.FacingWorld;
            Packet.bWalk = Movement.bWalk;
            Packet.bSprint = Movement.bSprint;
            Packet.bCrouch = Movement.bCrouch;
        }

        if (InputFrame.Aim.bActive)
        {
            const FBBBCharacterAimPacket &Aim = InputFrame.Aim.Data;
            Packet.AimTargetWorld = Aim.AimTargetWorld;
            Packet.bAim = Aim.bAim;
        }

        NetworkComponent.ServerSubmitControlPacket(MoveTemp(Packet));
    }

    const bool bHasAction = InputFrame.EquipSlot.bActive || InputFrame.Reload.bActive;

    if (bHasAction)
    {
        FBBBCharacterActionNetworkPacket Packet;
        Packet.bEquipSlot = InputFrame.EquipSlot.bActive;
        Packet.bReload = InputFrame.Reload.bActive;

        if (InputFrame.EquipSlot.bActive)
        {
            Packet.EquipSlot = InputFrame.EquipSlot.Data.Slot;
        }

        NetworkComponent.ServerSubmitActionPacket(MoveTemp(Packet));
    }

    // 网络组件已经取得完整副本，此后这些槽位可以接收下一帧输入
    InputFrame.Movement.Consume();
    InputFrame.Aim.Consume();
    InputFrame.Fire.Consume();
    InputFrame.EquipSlot.Consume();
    InputFrame.Reload.Consume();
}
