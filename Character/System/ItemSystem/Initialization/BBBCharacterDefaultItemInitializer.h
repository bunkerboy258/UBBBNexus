#pragma once

class UObject;
class FBBBCharacterItemStorage;
struct FBBBCharacterItemConfig;
struct FBBBCharacterItemRuntimeData;

/** 根据角色配置建立出生背包和快捷访问绑定 */
class FBBBCharacterDefaultItemInitializer final
{
public:

    /**
     * 建立角色出生物品
     * @param ItemData	角色物品黑板
     * @param Storage	背包写入器
     * @param ItemOuter	物品实例生命周期所有者
     * @param InItemConfig	角色物品配置
     */
    void Initialize(
        FBBBCharacterItemRuntimeData &ItemData,
        FBBBCharacterItemStorage &Storage,
        UObject &ItemOuter,
        const FBBBCharacterItemConfig &InItemConfig) const;
};
