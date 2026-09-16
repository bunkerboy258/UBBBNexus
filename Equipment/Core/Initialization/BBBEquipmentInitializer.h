#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;

/** 在配置注入后装配装备组件与运行数据 */
class ABBB_EVAC_API FBBBEquipmentInitializer final
{
public:
    static bool Initialize(ABBBEquipmentInstance &Instance);
};
