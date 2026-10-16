#pragma once
struct FBBBHealthPackRuntimeData;
class UBBBHealthPackDefinition;
class ABBBCharacter;
/** 本次调用内的依赖 */
struct FBBBHealthPackUseUpdateContext final
{
    FBBBHealthPackRuntimeData &Data;
    const UBBBHealthPackDefinition &Definition;
    ABBBCharacter &Character;
};
