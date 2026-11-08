#pragma once
class ABBBCharacter;
struct FBBBCharacterRuntimeData;
struct FBBBCharacterPickupConfig;
/** 本次角色世界拾取更新的临时依赖 */
struct FBBBCharacterPickupUpdateContext final
{
    /** 唯一角色运行时根 */
    ABBBCharacter &Character;
    /** 角色唯一运行时黑板 */
    FBBBCharacterRuntimeData &Data;
    /** 自动拾取的静态配置 */
    const FBBBCharacterPickupConfig &Config;
};
