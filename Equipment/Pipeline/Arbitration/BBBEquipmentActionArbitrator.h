#pragma once
#include "CoreMinimal.h"

struct FBBBEquipmentCommand;
struct FBBBEquipmentRuntimeData;
class UBBBEquipmentDefinition;

/** 装备自身操作条件与阶段仲裁 */
class FBBBEquipmentActionArbitrator final
{
public:
    /**
     * 判断本地命令是否可执行
     * @param Command	待执行命令
     * @param Runtime	装备事实
     * @param Definition	当前多态配置
     * @param WorldTime	当前世界时间
     * @param bActive	是否活动
     * @return 是否批准
     */
    static bool CanExecute(const FBBBEquipmentCommand &Command, const FBBBEquipmentRuntimeData &Runtime,
        const UBBBEquipmentDefinition &Definition, float WorldTime, bool bActive);
};
