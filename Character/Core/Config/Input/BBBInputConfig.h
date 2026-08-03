#pragma once
#include "CoreMinimal.h"
#include "BBBInputConfig.generated.h"
class UInputAction;

USTRUCT(BlueprintType)
//定义输入规范化与短时容错窗口
struct FBBBInputPipelineConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MoveDeadZone = 0.05f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MoveFlickerBufferTime = 0.05f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ActionBufferTime = 0.15f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float FireRaisedGraceTime = 0.2f;
};

USTRUCT(BlueprintType)
//聚合输入处理参数与角色输入动作
struct FBBBCharacterInputConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBInputPipelineConfig Pipeline;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> FireAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> PrecisionAimAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> SprintAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> ReloadAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> EquipSlot1Action;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> EquipSlot2Action;
};
