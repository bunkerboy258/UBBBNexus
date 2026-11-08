#pragma once

class ABBBEquipment;

/** 装备公共关闭骨架 */
class FBBBEquipmentShutdown
{
public:
    virtual ~FBBBEquipmentShutdown() = default;

    /** @param Equipment	失活或销毁的装备 @return 无 */
    void Shutdown(ABBBEquipment &Equipment) const;

protected:
    /** @param Equipment	需要清理的具体实例 @return 无 */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const = 0;
};
