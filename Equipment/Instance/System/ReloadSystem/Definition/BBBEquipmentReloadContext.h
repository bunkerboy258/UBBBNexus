#pragma once

#include "CoreMinimal.h"

class FBBBCharacterExternalAPI;

/** 换弹片段执行时可访问的装备事实 */
struct FBBBEquipmentReloadContext
{
    FBBBCharacterExternalAPI &CharacterAPI;
    int32 &LoadedAmmo;
    const int32 AmmoCapacity;
    bool &bIsReloading;
    bool &bMagazineDetached;
    int32 &ReloadSequence;
    int32 Sequence;
    bool bIsMirror;
};
