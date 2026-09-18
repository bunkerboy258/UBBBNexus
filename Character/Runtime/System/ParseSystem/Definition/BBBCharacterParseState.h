#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterControlState.h"
#include "BBBCharacterParseState.generated.h"

class ABBBEquipment;
class FBBBCharacterParseSystem;
class FBBBCharacterInputProcessor;

/** 包解析过程需要保留的跨帧操作事实 */
USTRUCT()
struct FBBBCharacterParseState
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterInputProcessor;
    friend struct FBBBCharacterMontagePacket;

    UPROPERTY()
    TWeakObjectPtr<ABBBEquipment> ReloadEquipment;

    UPROPERTY()
    TObjectPtr<ABBBEquipment> SelectedEquipment = nullptr;

    int32 ReloadSequence = INDEX_NONE;
    int32 LastCompletedReloadSequence = INDEX_NONE;
    int32 CancelReloadSequence = INDEX_NONE;
    bool bMagazineDetached = false;
    bool bEndQueued = false;
    bool bRestoreMode = false;
    bool bFire = false;
    bool bReload = false;

    FBBBCharacterControlFacts Control;
};
