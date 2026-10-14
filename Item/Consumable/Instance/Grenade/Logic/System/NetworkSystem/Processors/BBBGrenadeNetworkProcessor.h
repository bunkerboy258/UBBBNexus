#pragma once
struct FBBBGrenadeRuntimeData;
struct FBBBGrenadeNetworkUpdateContext;
/** 手雷Network处理器 */
class FBBBGrenadeNetworkProcessor final
{
public:
    /** @param Data 唯一黑板 @return 无 仅清除本领域状态 */
    static void Clear(FBBBGrenadeRuntimeData &Data);
    /** @param Context	本次领域上下文 @return 无 */
    void Update(FBBBGrenadeNetworkUpdateContext &Context) const;
};
