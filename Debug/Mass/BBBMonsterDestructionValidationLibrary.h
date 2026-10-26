#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Mass/EntityHandle.h"
#include "BBBMonsterDestructionValidationLibrary.generated.h"

/** 为明确的 PIE 测试群体提供毁伤对照 仍由正式输入维护实体事实 */
UCLASS(meta=(DisplayName="僵尸毁伤验收工具"))
class ABBB_EVAC_API UBBBMonsterDestructionValidationLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * 提交本轮测试玩家的八区毁伤及可选断臂贡献
     * @param WorldContext	当前 PIE 世界
     * @param Entities	本轮测试创建的实体句柄
     * @param RegionCount	按配置顺序开启的区域数量 零至八
     * @param bSeverLeftArm	同时贡献足够损毁左臂的有效伤害
     * @param TorsoDamage	本轮测试额外提交的躯干伤害
     * @return 成功投递的数量
     */
    UFUNCTION(BlueprintCallable, Category="BBB|验证", meta=(DisplayName="设置测试群体毁伤"))
    static int32 SetPopulationDestruction(UObject* WorldContext, const TArray<FMassEntityHandle>& Entities,
        int32 RegionCount, bool bSeverLeftArm, float TorsoDamage);

    /**
     * 读取所选实体的毁伤事实与实际组织组件 空数组读取当前世界全部小怪
     * @param WorldContext	当前 PIE 世界
     * @param Entities	明确的实体句柄
     * @return 含网络身份 八区强度 断点与组织组件的 JSON
     */
    UFUNCTION(BlueprintCallable, Category="BBB|验证", meta=(DisplayName="读取测试群体毁伤"))
    static FString InspectPopulationDestruction(UObject* WorldContext, const TArray<FMassEntityHandle>& Entities);
};
