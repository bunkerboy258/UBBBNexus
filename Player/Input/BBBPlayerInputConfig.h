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

    /** 按住维持与松开取消的救援输入动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "救援输入动作"))
    TObjectPtr<UInputAction> RescueAction;

    /** 按数组索引对应前序快捷槽位的输入动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "物品快捷槽位输入动作"))
    TArray<TObjectPtr<UInputAction>> ItemSlotActions;


};
