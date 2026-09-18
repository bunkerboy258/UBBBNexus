#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterControlInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterEquipmentInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterAnimationInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterRestoreInput.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"
#include "BBBInputRuntimeData.generated.h"
class FBBBCharacterInput;
class FBBBInputPipeline;
class FBBBArbitrationPipeline;

/** 输入入口持有的待消费数据与当前批次 */
USTRUCT()
struct FBBBInputBatch
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FBBBCharacterEquipInput> Equip;
    UPROPERTY()
    TArray<FBBBCharacterFireInput> Fire;
    UPROPERTY()
    TArray<FBBBCharacterReloadInput> Reload;
    UPROPERTY()
    TArray<FBBBEquipmentActionEvent> Results;
    UPROPERTY()
    TArray<FBBBCharacterReloadAnimationInput> Notifications;
    UPROPERTY()
    TArray<FBBBCharacterMontagePacket> Montages;
    UPROPERTY()
    TArray<FBBBCharacterRestoreInput> Restores;
    UPROPERTY()
    TArray<FBBBPlayerCameraInput> Camera;
};

/** 输入收件箱不在帧末清理以保留动画后装备提交的结果 */
USTRUCT()
struct FBBBInputRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterInput;
    friend class FBBBInputPipeline;
    friend class FBBBArbitrationPipeline;

    UPROPERTY()
    FBBBCharacterControlInput Control;
    UPROPERTY()
    FBBBCharacterControlInput FrameControl;
    UPROPERTY()
    FBBBInputBatch Pending;
    UPROPERTY()
    FBBBInputBatch Frame;
};
