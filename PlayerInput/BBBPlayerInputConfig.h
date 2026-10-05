#pragma once
#include "CoreMinimal.h"
#include "BBBPlayerInputConfig.generated.h"
class UInputAction;

USTRUCT(BlueprintType)
//聚合输入处理参数与角色输入动作
struct FBBBPlayerInputConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "移动输入死区"))
    float MoveDeadZone = 0.05f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "移动输入动作"))
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "视角输入动作"))
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "开火输入动作"))
    TObjectPtr<UInputAction> FireAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "精确瞄准输入动作"))
    TObjectPtr<UInputAction> PrecisionAimAction;

    /** 跑步输入动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "奔跑输入动作"))
    TObjectPtr<UInputAction> RunAction;

    /** 蹲伏输入动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "蹲伏输入动作"))
    TObjectPtr<UInputAction> CrouchAction;

    /** 跳跃输入动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "跳跃输入动作"))
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "换弹输入动作"))
    TObjectPtr<UInputAction> ReloadAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "装备栏位 1 输入动作"))
    TObjectPtr<UInputAction> EquipSlot1Action;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "装备栏位 2 输入动作"))
    TObjectPtr<UInputAction> EquipSlot2Action;


};
