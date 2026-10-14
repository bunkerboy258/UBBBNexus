#pragma once
/** 独立网络交接生命周期 */
struct FBBBThrowableNetworkState final
{
    /** 当前终态是否已交接可靠通道 */
    bool bPublished = false;
};
