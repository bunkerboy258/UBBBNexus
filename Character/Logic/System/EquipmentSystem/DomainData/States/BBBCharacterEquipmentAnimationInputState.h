#pragma once
#include "CoreMinimal.h"
class ABBBEquipment;
/** 角色动画通知等待装备通信阶段转交的数据 */
struct FBBBCharacterEquipmentAnimationInputState final
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
private:
    friend struct FBBBCharacterEquipmentDomainState;
    FBBBCharacterEquipmentAnimationInputState() = default;
};
