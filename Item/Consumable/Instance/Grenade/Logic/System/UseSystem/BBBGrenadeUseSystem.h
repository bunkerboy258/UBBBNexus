#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeUseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeLaunchProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeFinishProcessor.h"
class ABBBGrenadeConsumable;
struct FBBBGrenadeRuntimeData;
class FBBBGrenadeInitializer;

/** 手雷Use系统调度根 */
class FBBBGrenadeUseSystem final
{
public:
    /** @return 无 */
    void Update();

private:
    friend class FBBBGrenadeInitializer;
    /** @param InActor	所属实例 @param InData	唯一黑板 @return 无 */
    void Initialize(ABBBGrenadeConsumable &InActor, FBBBGrenadeRuntimeData &InData);
    ABBBGrenadeConsumable *Actor = nullptr;
    FBBBGrenadeRuntimeData *Data = nullptr;
    FBBBGrenadeUseProcessor UseProcessor;
    FBBBGrenadeLaunchProcessor LaunchProcessor;
    FBBBGrenadeFinishProcessor FinishProcessor;
};
