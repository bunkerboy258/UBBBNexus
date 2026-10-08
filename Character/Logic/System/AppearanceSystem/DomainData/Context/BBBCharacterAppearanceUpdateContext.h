#pragma once
class ABBBCharacter;
struct FBBBCharacterRuntimeData;
struct FBBBCharacterAppearanceConfig;
class UBBBItemCatalog;

/** 外观更新的栈上依赖 物品领域只读 */
struct FBBBCharacterAppearanceUpdateContext final
{
    /** 所属角色 */
    ABBBCharacter &Character;
    /** 唯一聚合黑板 */
    FBBBCharacterRuntimeData &Data;
    /** 基础配置 */
    const FBBBCharacterAppearanceConfig &Config;
    /** 外侧统一物品目录 */
    const UBBBItemCatalog *Catalog;
    /** 主管线确定的执行路径 */
    bool bIsMirror;
};
