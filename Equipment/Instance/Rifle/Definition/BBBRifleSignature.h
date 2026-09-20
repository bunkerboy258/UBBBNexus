#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Runtime/BBBEquipmentInputFrame.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Runtime/BBBEquipmentRuntimeContext.h"

class UBBBRifleDefinition;

/** 步枪装备完成事实时使用的输入包 */
struct FBBBRifleFactInput
{
    FBBBEquipmentActionFact Fact;
};

/** 步枪装备动作输入包 */
struct FBBBRifleEquipInput
{
    int32 Sequence = INDEX_NONE;
};

/** 步枪开火输入包 */
struct FBBBRifleFireInput
{
    int32 Sequence = INDEX_NONE;
};

/** 步枪开始换弹输入包 */
struct FBBBRifleReloadInput
{
    int32 Sequence = INDEX_NONE;
};

/** 步枪弹匣脱离输入包 */
struct FBBBRifleDetachMagazineInput
{
    int32 Sequence = INDEX_NONE;
};

/** 步枪弹匣装入输入包 */
struct FBBBRifleLoadMagazineInput
{
    int32 Sequence = INDEX_NONE;
};

/** 步枪换弹打断输入包 */
struct FBBBRifleCancelReloadInput
{
    int32 Sequence = INDEX_NONE;
};

/** 步枪的跨帧事实黑板 */
struct FBBBRifleState
{
    int32 AmmoCapacity = 0;

    int32 LoadedAmmo = 0;

    int32 FireSequence = 0;

    int32 ReloadSequence = INDEX_NONE;

    float LastFireTimeSeconds = -1000.0f;

    bool bIsReloading = false;

    bool bMagazineDetached = false;
};

/** 步枪静态输入签名与唯一解析逻辑 */
struct FBBBRifleSignature final
{
    using FState = FBBBRifleState;
    using FInputFrame = TBBBEquipmentInputFrame<
        FBBBRifleFactInput,
        FBBBRifleEquipInput,
        FBBBRifleFireInput,
        FBBBRifleReloadInput,
        FBBBRifleDetachMagazineInput,
        FBBBRifleLoadMagazineInput,
        FBBBRifleCancelReloadInput>;

    static void Initialize(FState &State, const UBBBRifleDefinition &Definition);

    static void Parse(FState &State, FInputFrame &InputFrame, FBBBEquipmentRuntimeContext &Context);
};
