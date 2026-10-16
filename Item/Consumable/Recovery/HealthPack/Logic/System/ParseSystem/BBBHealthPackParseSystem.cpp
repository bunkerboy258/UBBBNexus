#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/ParseSystem/BBBHealthPackParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/RuntimeData/BBBHealthPackRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/ParseSystem/DomainData/Context/BBBHealthPackParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/ParseSystem/Processors/BBBHealthPackParseProcessor.h"
void FBBBHealthPackParseSystem::Initialize(FBBBHealthPackRuntimeData &InData)
{
    Data = &InData;
}
void FBBBHealthPackParseSystem::Update()
{
    if (!Data)
    {
        return;
    }
    FBBBHealthPackParseUpdateContext Context{*Data};
    FBBBHealthPackParseProcessor{}.Update(Context);
}
void FBBBHealthPackParseSystem::Shutdown()
{
    Data = nullptr;
}
void FBBBHealthPackParseSystem::Submit(FBBBHealthPackPrimaryLocalControlPacket Packet)
{
    if (Data)
    {
        auto &Slot = Data->Parse.InputState.Primary;
        Slot.Packet.Stages.Append(Packet.Stages);
        Slot.bActive = true;
    }
}
void FBBBHealthPackParseSystem::Submit(FBBBHealthPackFinishLocalControlPacket Packet)
{
    if (Data)
    {
        auto &Slot = Data->Parse.InputState.Finish;
        Slot.Packet = MoveTemp(Packet);
        Slot.bActive = true;
    }
}
