#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/RuntimeData/BBBThrowableRuntimeData.h"
class ABBBThrowable;
/** 输入槽位唯一写入与解析处理器 */
class ABBB_EVAC_API FBBBThrowableParseProcessor final
{
public:
    /** @param Actor	当前投掷物 @return 无 */
    void Update(ABBBThrowable& Actor) const;
    /** @param Data 唯一黑板 @param Packet 独立命中贡献 @return 无 */
    static void Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableHitLocalControlPacket Packet);
    /** @param Data	唯一黑板 @param Packet	释放输入 @return 是否接受 */
    static void Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableLaunchLocalControlPacket Packet);
    /** @param Data	唯一黑板 @param Packet	碰撞输入 @return 是否接受 */
    static void Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableDetonateLocalControlPacket Packet);
    /** @param Data	唯一黑板 @param Packet	终态事实 @return 是否接受 */
    static void Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableExplosionAuthorityFactPacket Packet);
    /** @param Data	唯一黑板 @param Packet	真实碰撞输入 @return 无 */
    static void Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableBounceLocalControlPacket Packet);
    /** @param Data	唯一黑板 @param Packet	远端碰撞结果 @return 无 */
    static void Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableBounceAuthorityFactPacket Packet);
};
