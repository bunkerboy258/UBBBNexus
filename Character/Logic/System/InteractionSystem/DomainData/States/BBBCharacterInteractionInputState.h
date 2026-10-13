#pragma once
#include "CoreMinimal.h"
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
    /** 本帧通用主行为 */
    bool bPrimary = false;
    /** 本帧通用次行为 */
    bool bSecondary = false;
    /** 释放通知对应的实际持有实例 */
    TArray<TWeakObjectPtr<AActor>> ReleaseRecipients;
    /** 释放通知的持有代次 */
    TArray<uint64> ReleaseGenerations;
    /** 释放通知的使用序号 */
    TArray<int32> ReleaseTokens;
    /** 结束通知对应的实际持有实例 */
    TArray<TWeakObjectPtr<AActor>> FinishRecipients;
    /** 结束通知的持有代次 */
    TArray<uint64> FinishGenerations;
    /** 结束通知的使用序号 */
    TArray<int32> FinishTokens;
private:
    friend struct FBBBCharacterInteractionDomainState;
    FBBBCharacterInteractionInputState() = default;
};
