#pragma once

#include "CoreMinimal.h"

class UBBBItemInstance;
struct FBBBCharacterBackpackState;

/** 维护角色唯一背包及快捷访问绑定 */
class FBBBCharacterItemStorage final
{
public:

    /**
     * 将物品放入第一个空背包槽位
     * @param Backpack	角色唯一背包
     * @param ItemInstance	待加入物品实例
     * @return 是否成功加入背包
     */
    bool AddItem(
        FBBBCharacterBackpackState &Backpack,
        UBBBItemInstance &ItemInstance) const;

    /**
     * 将快捷操作槽位绑定到已有背包实例
     * @param Backpack	角色唯一背包
     * @param QuickAccessSlot	快捷操作槽位
     * @param ItemInstance	背包内物品实例
     * @return 是否成功建立绑定
     */
    bool BindQuickAccessItem(
        FBBBCharacterBackpackState &Backpack,
        int32 QuickAccessSlot,
        UBBBItemInstance &ItemInstance) const;
};
