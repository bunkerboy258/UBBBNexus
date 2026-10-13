#pragma once
class ABBBGrenadeConsumable;
class ABBBCharacter;
class UWorld;
class UBBBGrenadeDefinition;
struct FBBBGrenadeRuntimeData;

/** 手雷使用主管线的一次调用数据 */
struct FBBBGrenadeUseUpdateContext final
{
    ABBBGrenadeConsumable &Actor;
    ABBBCharacter &Character;
    UWorld &World;
    FBBBGrenadeRuntimeData &Data;
    const UBBBGrenadeDefinition &Definition;
    bool bIsMirror = true;
};
