#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/RuntimeData/BBBThrowableRuntimeData.h"
class ABBBThrowable;
/** 输入槽位唯一写入与解析处理器 */
class FBBBThrowableParseProcessor final
{
public:
    /** @param Actor	当前投掷物 @return 无 */
    void Update(ABBBThrowable& Actor) const;
    /** @param Data	唯一黑板 @param Packet	释放输入 @return 是否接受 */
    static bool Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableLaunchLocalControlPacket Packet);
    /** @param Data	唯一黑板 @param Packet	碰撞输入 @return 是否接受 */
    static bool Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableDetonateLocalControlPacket Packet);
    /** @param Data	唯一黑板 @param Packet	终态事实 @return 是否接受 */
    static bool Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableExplosionAuthorityFactPacket Packet);
};
