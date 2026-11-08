#pragma once

class ABBBEquipment;

/** 装备公共更新与时序骨架 */
class FBBBEquipmentUpdatePipeline
{
public:
    virtual ~FBBBEquipmentUpdatePipeline() = default;

    /** @param Equipment	目标装备 @return 无 */
    static void ConfigureTick(ABBBEquipment &Equipment);

    /** @param Equipment	本帧装备 @return 无 */
    void Update(ABBBEquipment &Equipment) const;

protected:
    /** @param Equipment	已初始化装备 @return 无 */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const = 0;
};
