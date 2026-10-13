#pragma once
class ABBBGrenadeConsumable;
class ABBBCharacter;
struct FBBBGrenadeRuntimeData;

/** 手雷网络观察的一次调用数据 */
struct FBBBGrenadeNetworkUpdateContext final
{
    ABBBGrenadeConsumable &Actor;
    ABBBCharacter &Character;
    FBBBGrenadeRuntimeData &Data;
};
