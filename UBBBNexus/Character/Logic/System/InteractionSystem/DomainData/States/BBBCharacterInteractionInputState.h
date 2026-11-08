#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Definitions/BBBCharacterUseFact.h"
class ABBBEquipment;
class AActor;
/** 角色动画通知等待装备通信阶段转交的数据 */
struct FBBBCharacterInteractionInputState final
{
    /** BeginAction通知的目标装备 */
    TArray<TWeakObjectPtr<ABBBEquipment>> BeginActionRecipients;
    /** BeginAction通知的实例标识 */
    TArray<int32> BeginActionTokens;
    /** EndAction通知的目标装备 */
    TArray<TWeakObjectPtr<ABBBEquipment>> EndActionRecipients;
    /** EndAction通知的实例标识 */
    TArray<int32> EndActionTokens;
    /** BeginContact通知的目标装备 */
    TArray<TWeakObjectPtr<ABBBEquipment>> BeginContactRecipients;
    /** BeginContact通知的实例标识 */
    TArray<int32> BeginContactTokens;
    /** EndContact通知的目标装备 */
    TArray<TWeakObjectPtr<ABBBEquipment>> EndContactRecipients;
    /** EndContact通知的实例标识 */
    TArray<int32> EndContactTokens;
    /** 本帧快速近战请求 */
    bool bQuickMelee = false;
    /** 命中窗口开始对应的动作序号 */
    TArray<int32> MeleeBeginTokens;
    /** 命中窗口结束对应的动作序号 */
    TArray<int32> MeleeEndTokens;
    /** 当帧收到的已成立释放结果 处理后立即清空 */
    TArray<FBBBCharacterUseFact> ReleaseResults;
    /** 显示通知绑定的持有代次 */
    TArray<uint64> ShowEquipmentGenerations;
    /** 隐藏通知绑定的持有代次 */
    TArray<uint64> HideEquipmentGenerations;
    /** 本帧通用主行为 */
    bool bPrimary = false;
    /** 本帧主行为的独立按下次数 */
    int32 PrimaryPressCount = 0;
    /** 本帧通用次行为 */
    bool bSecondary = false;
    /** 释放通知对应的实际持有实例 */
    TArray<TWeakObjectPtr<UObject>> ReleaseRecipients;
    /** 释放通知的持有代次 */
    TArray<uint64> ReleaseGenerations;
    /** 释放通知的使用序号 */
    TArray<int32> ReleaseTokens;
    /** 结束通知对应的实际持有实例 */
    TArray<TWeakObjectPtr<UObject>> FinishRecipients;
    /** 结束通知的持有代次 */
    TArray<uint64> FinishGenerations;
    /** 结束通知的使用序号 */
    TArray<int32> FinishTokens;
private:
    friend struct FBBBCharacterInteractionDomainState;
    FBBBCharacterInteractionInputState() = default;
};
