#pragma once
class ABBBWorldDropItem;
/** 掉落网络系统根 */
class FBBBWorldDropNetworkSystem final
{
public:
    /** @param Drop\t唯一运行时根 @return 无 组装并调度处理器 */
    void Update(ABBBWorldDropItem &Drop) const;
};
