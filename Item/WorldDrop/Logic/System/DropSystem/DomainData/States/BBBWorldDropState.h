#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/Definitions/BBBWorldDropSnapshot.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/Definitions/BBBWorldDropInitialization.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/Definitions/BBBWorldDropClaim.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Definitions/BBBCharacterPickupReceipt.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/Definitions/BBBWorldDropCompletion.h"
#include "BBBWorldDropState.generated.h"
/** 世界掉落物生命周期与待处理输入 */
USTRUCT()
struct FBBBWorldDropState final
{
    GENERATED_BODY()
    /** 唯一当前掉落与交付结果 */
    UPROPERTY()
    FBBBWorldDropSnapshot Snapshot;
    /** 生成后等待解析的初始化请求 */
    UPROPERTY()
    TArray<FBBBWorldDropInitialization> Initializations;
    /** 待受理的角色领取请求 */
    UPROPERTY()
    TArray<FBBBWorldDropClaim> Claims;
    /** 待核对的角色入包结果 */
    UPROPERTY()
    TArray<FBBBWorldDropCompletion> Completions;

};
