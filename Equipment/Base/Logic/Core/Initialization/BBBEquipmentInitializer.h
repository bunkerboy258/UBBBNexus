#pragma once

class ABBBEquipment;

/** 装配装备公共组件与具体实例 */
class FBBBEquipmentInitializer
{
public:
    virtual ~FBBBEquipmentInitializer() = default;

    /** @param Equipment	待初始化装备 @return 是否初始化成功 */
    bool Initialize(ABBBEquipment &Equipment) const;

protected:
    /** @param Equipment	已完成公共装配的装备 @return 具体实例是否初始化成功 */
    virtual bool InitializeInstance(ABBBEquipment &Equipment) const = 0;
};
