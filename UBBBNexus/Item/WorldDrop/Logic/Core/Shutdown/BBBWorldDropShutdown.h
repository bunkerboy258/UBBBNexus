#pragma once
class ABBBWorldDropItem;
/** 世界掉落物结束生命周期 */
class FBBBWorldDropShutdown final
{
public:
    /** @param Drop\t唯一运行时根 @return 无 */
    static void Shutdown(ABBBWorldDropItem &Drop);
};
