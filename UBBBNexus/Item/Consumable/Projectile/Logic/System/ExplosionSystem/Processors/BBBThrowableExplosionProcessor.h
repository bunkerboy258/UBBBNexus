#pragma once
class ABBBThrowable;
/** Explosion领域的唯一处理器 */
class FBBBThrowableExplosionProcessor final
{
public:
    /** @param Actor	当前投掷物 @return 无 */
    void Update(ABBBThrowable& Actor) const;
};
