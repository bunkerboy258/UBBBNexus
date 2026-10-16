#pragma once
struct FBBBGrenadeRuntimeData;
class UBBBGrenadeDefinition;
class ABBBCharacter;
/** 本次调用内的依赖 */
struct FBBBGrenadeUseUpdateContext final
{
    FBBBGrenadeRuntimeData &Data;
    const UBBBGrenadeDefinition &Definition;
    ABBBCharacter &Character;
};
