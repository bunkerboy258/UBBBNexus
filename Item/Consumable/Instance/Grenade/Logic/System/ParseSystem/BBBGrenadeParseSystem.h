#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include <type_traits>
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/Processors/BBBGrenadeParseProcessor.h"
class ABBBGrenadeConsumable;
struct FBBBGrenadeRuntimeData;
class FBBBGrenadeInitializer;

/** 手雷Parse系统调度根 */
class FBBBGrenadeParseSystem final
{
public:
    /** @return 无 */
    void Update();
    /** @param Packet	待合并的固定输入 @return 是否接受 */
    template<typename TPacket>
    bool Submit(TPacket Packet);

private:
    friend class FBBBGrenadeInitializer;
    /** @param InActor	所属实例 @param InData	唯一黑板 @return 无 */
    void Initialize(ABBBGrenadeConsumable &InActor, FBBBGrenadeRuntimeData &InData);
    ABBBGrenadeConsumable *Actor = nullptr;
    FBBBGrenadeRuntimeData *Data = nullptr;
    FBBBGrenadeParseProcessor ParseProcessor;
};

template<typename TPacket>
bool FBBBGrenadeParseSystem::Submit(TPacket Packet)
{
    return Data && FBBBGrenadeParseProcessor::Submit(*Data, MoveTemp(Packet));
}
