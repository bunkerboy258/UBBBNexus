#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/Processors/BBBRifleNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkComponent.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

void FBBBRifleNetworkProcessor::Encode(const FBBBRifleRuntimeData &Data, TArray<uint8> &Payload)
{
    const auto &State = Data.Action.ReadRifleActionState();
    int32 LoadedAmmo = State.LoadedAmmo;
    int32 FireSequence = State.FireSequence;
    int32 ReloadSequence = State.ReloadSequence;
    uint8 Reloading = State.bIsReloading ? 1 : 0;
    uint8 Detached = State.bMagazineDetached ? 1 : 0;
    Payload.Reset();
    FMemoryWriter Writer(Payload);
    Writer << LoadedAmmo;
    Writer << FireSequence;
    Writer << ReloadSequence;
    Writer << Reloading;
    Writer << Detached;
}

bool FBBBRifleNetworkProcessor::Submit(FBBBRifleRuntimeData &Data, const TArray<uint8> &Payload)
{
    // 固定格式必须完整到达 不接受部分结果或额外尾部数据
    if (!ensureMsgf(Payload.Num() == 3 * sizeof(int32) + 2 * sizeof(uint8), TEXT("步枪网络状态长度错误")))
    {
        return false;
    }

    FBBBRifleNetworkStatePacket Packet;
    uint8 Reloading = 0;
    uint8 Detached = 0;
    FMemoryReader Reader(Payload);
    Reader << Packet.LoadedAmmo;
    Reader << Packet.FireSequence;
    Reader << Packet.ReloadSequence;
    Reader << Reloading;
    Reader << Detached;
    if (!ensureMsgf(!Reader.IsError() && Packet.LoadedAmmo >= 0
        && Packet.LoadedAmmo <= Data.Action.ReadRifleActionState().AmmoCapacity
        && Packet.FireSequence >= 0 && Packet.ReloadSequence >= 0
        && Reloading <= 1 && Detached <= 1 && (!Detached || Reloading),
        TEXT("步枪网络状态字段无效")))
    {
        return false;
    }

    Packet.bIsReloading = Reloading != 0;
    Packet.bMagazineDetached = Detached != 0;
    Data.Parse.InputState.NetworkState.Packet = Packet;
    Data.Parse.InputState.NetworkState.bActive = true;
    return true;
}

void FBBBRifleNetworkProcessor::Update(FBBBRifleUpdateContext &Context)
{
    if (Context.Equipment.IsMirror())
    {
        return;
    }

    UBBBEquipmentNetworkComponent *Network = Context.Character.FindComponentByClass<UBBBEquipmentNetworkComponent>();
    if (!ensureMsgf(Network, TEXT("持有角色缺少装备网络组件")))
    {
        return;
    }

    TArray<uint8> Payload;
    Encode(Context.RuntimeData, Payload);
    Network->PublishState(&Context.Equipment, Payload);
}
