#pragma once
#include "CoreMinimal.h"

/** 背包主界面文案 物品名称与描述继续使用物品自身展示数据 */
namespace BBBItemText
{
inline const FText Backpack = NSLOCTEXT("BBBPlayerItems", "Backpack", "背包");
inline const FText Weapons = NSLOCTEXT("BBBPlayerItems", "Weapons", "武器");
inline const FText Character = NSLOCTEXT("BBBPlayerItems", "Character", "角色");
inline const FText Wearables = NSLOCTEXT("BBBPlayerItems", "Wearables", "穿戴");
inline const FText Consumables = NSLOCTEXT("BBBPlayerItems", "Consumables", "消耗品");
inline const FText Misc = NSLOCTEXT("BBBPlayerItems", "Misc", "杂物");
inline const FText Return = NSLOCTEXT("BBBPlayerItems", "Return", "返回");
inline const FText Health = NSLOCTEXT("BBBPlayerItems", "Health", "生命  {0} / {1}");
inline const FText PlacementRejected = NSLOCTEXT("BBBPlayerItems", "PlacementRejected", "此位置无法放置该物品");

/** @param Slot	角色提供的穿戴位置标识 @return 仅用于界面的中文位置名称 */
inline FText WearPosition(const FName Slot)
{
    static const TMap<FName, FText> Labels = {
        {TEXT("Helmet"), NSLOCTEXT("BBBPlayerItems", "Helmet", "头盔")},
        {TEXT("Body"), NSLOCTEXT("BBBPlayerItems", "Body", "上衣")},
        {TEXT("Vest"), NSLOCTEXT("BBBPlayerItems", "Vest", "护甲")},
        {TEXT("Boots"), NSLOCTEXT("BBBPlayerItems", "Boots", "靴子")},
        {TEXT("Backpack"), NSLOCTEXT("BBBPlayerItems", "BackpackPosition", "背包")},
        {TEXT("Gloves"), NSLOCTEXT("BBBPlayerItems", "Gloves", "手套")},
        {TEXT("Attachments"), NSLOCTEXT("BBBPlayerItems", "Attachments", "附件")},
        {TEXT("Flag"), NSLOCTEXT("BBBPlayerItems", "Flag", "徽记")},
        {TEXT("Legs"), NSLOCTEXT("BBBPlayerItems", "Legs", "裤装")},
        {TEXT("Belt"), NSLOCTEXT("BBBPlayerItems", "Belt", "腰带")}};
    if (const FText *Label = Labels.Find(Slot))
    {
        return *Label;
    }
    return Wearables;
}
}
