#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
struct FBBBCharacterRuntimeData;
struct FBBBCharacterAppearanceConfig;
class UBBBItemCatalog;
struct FBBBAppearanceResource;

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
    /** 本次是否需要材质维护和显示应用 */
    bool bPrepareDisplay = false;
    /** 请求资源是否完整可用 */
    bool bResourcesReady = true;
    /** 同一既成事实只报告首次失败 */
    bool bReportFailure = false;
    /** 与请求显示缓存逐项对应的静态资源 */
    TArray<const FBBBAppearanceResource *> Resources;
    /** 与基础回退缓存逐项对应的静态资源 */
    TArray<const FBBBAppearanceResource *> FallbackResources;
};
