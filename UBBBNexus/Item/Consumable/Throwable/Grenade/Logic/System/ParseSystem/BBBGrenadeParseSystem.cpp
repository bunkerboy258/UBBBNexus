#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/ParseSystem/BBBGrenadeParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/ParseSystem/DomainData/Context/BBBGrenadeParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/ParseSystem/Processors/BBBGrenadeParseProcessor.h"
void FBBBGrenadeParseSystem::Initialize(FBBBGrenadeRuntimeData &InData)
{
    Data = &InData;
}
void FBBBGrenadeParseSystem::Update()
{
    if (!Data)
    {
        return;
    }
    FBBBGrenadeParseUpdateContext Context{*Data};
    FBBBGrenadeParseProcessor{}.Update(Context);
}
void FBBBGrenadeParseSystem::Shutdown()
{
    Data = nullptr;
}
void FBBBGrenadeParseSystem::Submit(FBBBGrenadePrimaryLocalControlPacket Packet)
{
    if (Data)
    {
        auto &Slot = Data->Parse.InputState.Primary;
        Slot.Packet = MoveTemp(Packet);
        Slot.bActive = true;
    }
}
void FBBBGrenadeParseSystem::Submit(FBBBGrenadeFinishLocalControlPacket Packet)
{
    if (Data)
    {
        auto &Slot = Data->Parse.InputState.Finish;
        Slot.Packet = MoveTemp(Packet);
        Slot.bActive = true;
    }
}
