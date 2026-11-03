#pragma once
class ABBBWorldDropItem;
struct FBBBWorldDropRuntimeData;
/** 本次网络结果投送更新的临时依赖 */
struct FBBBWorldDropNetworkUpdateContext final
{
    /** 唯一世界运行时根 */
    ABBBWorldDropItem &Drop;
    /** 唯一运行时黑板 */
    FBBBWorldDropRuntimeData &Data;
};
