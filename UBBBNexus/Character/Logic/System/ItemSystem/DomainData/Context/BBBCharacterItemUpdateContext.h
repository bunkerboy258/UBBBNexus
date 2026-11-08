#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
struct FBBBCharacterRuntimeData;
struct FBBBCharacterItemConfig;

/** 本次物品更新的栈上依赖 */
struct FBBBCharacterItemUpdateContext final
{
    /** 物品实例所属角色 */
    ABBBCharacter &Character;

    /** 角色唯一聚合黑板 */
    FBBBCharacterRuntimeData &RuntimeData;

    /** 本领域静态容量与快捷区域配置 */
    const FBBBCharacterItemConfig &Config;
};
