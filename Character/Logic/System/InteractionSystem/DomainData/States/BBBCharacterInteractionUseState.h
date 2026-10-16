#pragma once
#include "CoreMinimal.h"
#include "Mass/EntityHandle.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Definitions/BBBCharacterUseFact.h"
#include "BBBCharacterInteractionUseState.generated.h"

/** 当前持有物品的交互权限与通知标识 */
USTRUCT()
struct FBBBCharacterInteractionUseState final
{
    GENERATED_BODY()
    /** 当前动作是否阻止背包操作与主动切换 */
    bool bBusy = false;
    /** 当前实例的只读使用序号 用于拒绝过期通知 */
    int32 UseSerial = 0;
    /** 已成立的角色使用结果 */
    UPROPERTY(Transient)
    FBBBCharacterUseFact Fact;
    /** 待还原的角色使用结果 */
    UPROPERTY(Transient)
    FBBBCharacterUseFact PendingFact;
    /** 本次使用的库存身份 */
    FGuid Instance;
    /** ItemSystem 拥有的当次使用对象 */
    TWeakObjectPtr<class UBBBConsumable> Runtime;
    /** 本次使用的静态定义 */
    UPROPERTY(Transient)
    TObjectPtr<class UBBBConsumableDefinition> Definition = nullptr;
    /** 房主已生成世界实体的使用序号 */
    int32 SpawnedSerial = 0;
    /** 本次快速近战命中窗口 */
    bool bMeleeContact = false;
    /** 本次近战上帧是否具有有效手部姿态 */
    bool bHasMeleePose = false;
    /** 上帧接触段起点 */
    FVector PreviousMeleeBase = FVector::ZeroVector;
    /** 上帧接触段末端 */
    FVector PreviousMeleeTip = FVector::ZeroVector;
    /** 本次窗口已命中的实体 */
    TArray<FMassEntityHandle> MeleeEntities;
    /** 本次窗口已命中的 Actor */
    TArray<TWeakObjectPtr<AActor>> MeleeActors;
    /** 当前持有物品是否允许主动切换或解除装备 */
    bool bSwitchAllowed = true;
    /** 当前持有物品是否允许 G 快捷消耗品打断 */
    bool bQuickUseAllowed = true;
};
