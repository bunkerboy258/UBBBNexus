#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterControlInput.h"
#include "BBBCharacterOperationState.generated.h"
class ABBBEquipment;
class FBBBArbitrationPipeline;

/** 管线持有的操作生命周期与本帧裁决 */
USTRUCT()
struct FBBBCharacterOperationState
{
    GENERATED_BODY()

private:
    friend class FBBBArbitrationPipeline;
    friend struct FBBBEquipBehavior;
    friend struct FBBBFireBehavior;
    friend struct FBBBReloadBehavior;
    friend struct FBBBCharacterMontagePacket;
    friend struct FBBBCharacterRestoreInput;

    UPROPERTY()
    TWeakObjectPtr<ABBBEquipment> ReloadEquipment;

    int32 ReloadSequence = INDEX_NONE;
    int32 LastCompletedReloadSequence = INDEX_NONE;
    int32 CancelReloadSequence = INDEX_NONE;
    bool bMagazineDetached = false;
    bool bEndQueued = false;
    bool bRestoreMode = false;

    UPROPERTY()
    FBBBCharacterControlInput Control;

    UPROPERTY()
    TObjectPtr<ABBBEquipment> SelectedEquipment = nullptr;

    bool bFire = false;
    bool bReload = false;
};
