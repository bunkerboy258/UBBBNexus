#pragma once
class ABBBWorldDropItem;
struct FBBBWorldDropRuntimeData;
/** 本次掉落物业务更新的临时依赖 */
struct FBBBWorldDropUpdateContext final
{
    /** 唯一世界运行时根 */
    ABBBWorldDropItem &Drop;
    /** 唯一运行时黑板 */
    FBBBWorldDropRuntimeData &Data;
    /** 主管线确定的本次业务执行路径 */
    bool bCausal = false;
};
