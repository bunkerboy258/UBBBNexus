#pragma once

class ABBBCharacter;
class UBBBCharacterConfig;
struct FBBBCharacterRuntimeData;

/** 本次体力处理的栈上依赖 */
struct FBBBCharacterStaminaUpdateContext final
{
    /** 当前角色 */
    ABBBCharacter &Character;

    /** 唯一聚合黑板 */
    FBBBCharacterRuntimeData &Data;

    /** 当前角色配置 */
    const UBBBCharacterConfig &Config;
};
