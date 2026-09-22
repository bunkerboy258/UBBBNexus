#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Mass/EntityHandle.h"
#include "BBBWork/UBBBNexus/MonsterMass/Input/MonsterDamageRequest.h"

#include "MonsterDamageInput.generated.h"

/** 武器与其它外部系统共用的游戏线程伤害入口 */
UCLASS()
class ABBB_EVAC_API UMonsterDamageInput final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * 向目标关联的实体提交伤害 不立即扣血
     * @param Target		目标表现演员
     * @param Request		单次伤害及其来源
     * @return 请求是否已进入队列
     */
    UFUNCTION(BlueprintCallable, Category = "Monster|Damage")
    static bool SubmitDamage(AActor* Target, const FMonsterDamageRequest& Request);

    /**
     * 向没有表现演员的实体提交伤害
     * @param World		实体所属世界
     * @param Entity		包含代次的实体句柄
     * @param Request		单次伤害及其来源
     * @return 请求是否已进入队列
     */
    static bool SubmitDamageToEntity(UWorld* World, FMassEntityHandle Entity, const FMonsterDamageRequest& Request);

    /**
     * 读取已经消费完毕的生命状态 不预扣排队伤害
     * @param Target		目标表现演员
     * @param CurrentHealth	当前生命值
     * @param MaxHealth		最大生命值
     * @param bDead		是否已经死亡 无效目标也返回真
     * @return 是否成功取得实体状态
     */
    UFUNCTION(BlueprintPure, Category = "Monster|Damage")
    static bool TryGetHealth(AActor* Target, float& CurrentHealth, float& MaxHealth, bool& bDead);
};
