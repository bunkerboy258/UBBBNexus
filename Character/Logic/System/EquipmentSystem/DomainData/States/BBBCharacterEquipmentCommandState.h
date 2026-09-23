#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"
#include "BBBCharacterEquipmentCommandState.generated.h"

class ABBBEquipment;

/** 角色装备领域等待消费的命令状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentCommandState final
{
    GENERATED_BODY()

    /** 是否存在待执行开火命令 */
    UPROPERTY()
    bool bActivateFire = false;

    /** 是否存在待执行换弹命令 */
    UPROPERTY()
    bool bActivateReload = false;

    /** 是否提交了副操作状态 */
    UPROPERTY()
    bool bSecondarySubmitted = false;

    /** 最后一次副操作状态 */
    UPROPERTY()
    bool bSecondaryActive = false;

    /** 等待镜像恢复的动作事实 */
    UPROPERTY()
    TArray<FBBBEquipmentActionFact> PendingFacts;

    /** 等待创建镜像装备的配置 */
    UPROPERTY()
    TSubclassOf<ABBBEquipment> PendingEquipmentClass;
};
