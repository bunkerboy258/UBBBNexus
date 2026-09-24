#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBRifleParseProcessor::Update(FBBBRifleUpdateContext &Context)
{
    auto &Input = Context.RuntimeData.Parse.InputState;
    if (Context.Equipment.IsMirror())
    {
        Input.Fire.bActive = false;
        Input.Reload.bActive = false;
        Input.DetachMagazine.bActive = false;
        Input.LoadMagazine.bActive = false;
        Input.InterruptReload.bActive = false;
        return;
    }

    Input.NetworkState.bActive = false;
}

void FBBBRifleParseProcessor::Clear(FBBBRifleRuntimeData &Data)
{
    auto &Input = Data.Parse.InputState;
    Input.Equip.bActive = false;
    Input.Fire.bActive = false;
    Input.Reload.bActive = false;
    Input.DetachMagazine.bActive = false;
    Input.LoadMagazine.bActive = false;
    Input.InterruptReload.bActive = false;
    Input.NetworkState.bActive = false;
}

void FBBBRifleParseProcessor::Submit(FBBBRifleRuntimeData &Data, const FBBBRifleEquipPacket &Packet)
{
    Data.Parse.InputState.Equip.Packet = Packet;
    Data.Parse.InputState.Equip.bActive = true;
}

void FBBBRifleParseProcessor::Submit(FBBBRifleRuntimeData &Data, const FBBBRifleFirePacket &Packet)
{
    Data.Parse.InputState.Fire.Packet = Packet;
    Data.Parse.InputState.Fire.bActive = true;
}

void FBBBRifleParseProcessor::Submit(FBBBRifleRuntimeData &Data, const FBBBRifleReloadPacket &Packet)
{
    Data.Parse.InputState.Reload.Packet = Packet;
    Data.Parse.InputState.Reload.bActive = true;
}

void FBBBRifleParseProcessor::Submit(FBBBRifleRuntimeData &Data, const FBBBRifleDetachMagazinePacket &Packet)
{
    Data.Parse.InputState.DetachMagazine.Packet = Packet;
    Data.Parse.InputState.DetachMagazine.bActive = true;
}

void FBBBRifleParseProcessor::Submit(FBBBRifleRuntimeData &Data, const FBBBRifleLoadMagazinePacket &Packet)
{
    Data.Parse.InputState.LoadMagazine.Packet = Packet;
    Data.Parse.InputState.LoadMagazine.bActive = true;
}

void FBBBRifleParseProcessor::Submit(FBBBRifleRuntimeData &Data, const FBBBRifleInterruptReloadPacket &Packet)
{
    Data.Parse.InputState.InterruptReload.Packet = Packet;
    Data.Parse.InputState.InterruptReload.bActive = true;
}
