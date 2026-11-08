#pragma once
class ABBBWorldDropItem;
/** 世界掉落物唯一更新调度 */
class FBBBWorldDropUpdatePipeline final
{
public:
    /** @param Drop\t唯一运行时根 @return 无 */
    static void Update(ABBBWorldDropItem &Drop);
};
