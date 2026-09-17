#pragma once

#include "CoreMinimal.h"

class FBBBCharacterInput;

/** 换弹片段执行时可访问的装备事实 */
struct FBBBEquipmentReloadContext
{
    FBBBCharacterInput &CharacterAPI;
    int32 &LoadedAmmo;
    const int32 AmmoCapacity;
    bool &bIsReloading;
    bool &bMagazineDetached;
    int32 &ReloadSequence;
    int32 Sequence;
    bool bIsMirror;
};
