#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterControlFacts.h"
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
    friend class FBBBBehaviorRuleDispatcher;
    friend struct FBBBMoveBehavior;
    friend struct FBBBViewBehavior;
    friend struct FBBBAimBehavior;
    friend struct FBBBWalkBehavior;
    friend struct FBBBCrouchBehavior;
    friend struct FBBBJumpBehavior;
    friend struct FBBBSprintBehavior;
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
    uint8 ActiveBehaviorGroups = 0;

    UPROPERTY()
    FBBBCharacterControlFacts Control;

    UPROPERTY()
    TObjectPtr<ABBBEquipment> SelectedEquipment = nullptr;

    bool bFire = false;
    bool bReload = false;
};
