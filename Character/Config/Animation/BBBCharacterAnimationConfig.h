#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "BBBCharacterAnimationConfig.generated.h"

class UAnimInstance;
/**
 * 配置动画系统从角色实际运动中识别表现事实所需的阈值
 */
USTRUCT(BlueprintType)
struct FBBBCharacterAnimationConfig
{
    GENERATED_BODY()
    /** 空手快速近战动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "空手快速近战"))
    TObjectPtr<class UAnimMontage> UnarmedQuickMeleeMontage = nullptr;
    /** 空手使用结束的短收势动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "空手使用结束收势"))
    TObjectPtr<class UAnimMontage> UseReturnMontage = nullptr;

    /** 倒地等待期间循环播放的全身动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "倒地保持动作"))
    TObjectPtr<class UAnimMontage> DownedHoldMontage = nullptr;

    /** 接受救援的全身动作 成功由动作内的逻辑通知提交输入 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "接受救援动作"))
    TObjectPtr<class UAnimMontage> ReceiveRescueMontage = nullptr;

    /** 救援者持续帮扶的全身动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "帮扶保持动作"))
    TObjectPtr<class UAnimMontage> HelpRescueMontage = nullptr;

    /** 获救后恢复站立的全身动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "站立起身动作"))
    TObjectPtr<class UAnimMontage> RecoverStandMontage = nullptr;

    /** 获救后恢复蹲伏的全身动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "蹲伏起身动作"))
    TObjectPtr<class UAnimMontage> RecoverCrouchMontage = nullptr;

    /** 移动或帮扶结束时覆盖全身动作并收势回到运动 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "生命动作退出收势"))
    TObjectPtr<class UAnimMontage> LifeActionReturnMontage = nullptr;

    /** 未装备专用动画层时链接的默认动画层 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|动画层", meta = (DisplayName = "默认动画层类"))
    TSubclassOf<UAnimInstance> DefaultAnimationLayerClass;

    /** 角色实际水平转速超过该值时生成对应方向的原地转身信号 单位为度每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|朝向", meta = (ClampMin = "0.0", DisplayName = "转身信号转速阈值"))
    float TurnSignalRateThreshold = 20.0f;

    /** 将角色离散旋转量转换为稳定动画转速所使用的低通时间 单位为秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|朝向", meta = (ClampMin = "0.001", DisplayName = "转速平滑时间"))
    float TurnRateSmoothingTime = 0.12f;

    /** 动画转速每秒允许变化的最大幅度 单位为度每平方秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|朝向", meta = (ClampMin = "0.0", DisplayName = "最大转速变化速度"))
    float MaxTurnRateChangeSpeed = 720.0f;

    /** 额外瞄准上下与左右偏移的最大绝对角度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|冲量", meta = (DisplayName = "瞄准冲量角度上限"))
    FVector2D RecoilLimitDegrees = FVector2D(30.0f, 30.0f);

};
