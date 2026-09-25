#pragma once

class ABBBEquipment;

/** 装配装备公共组件与具体实例 */
class FBBBEquipmentInitializer final
{
public:
    /** @param Equipment	待初始化装备 @return 是否初始化成功 */
    static bool Initialize(ABBBEquipment &Equipment);
};
