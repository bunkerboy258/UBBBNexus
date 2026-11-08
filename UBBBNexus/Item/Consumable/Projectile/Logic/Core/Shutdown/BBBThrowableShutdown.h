#pragma once
class ABBBThrowable;
/** 投掷物Shutdown入口 */
class FBBBThrowableShutdown final
{
public:
    /** @param Actor	当前投掷物 @return 无 */
    void Shutdown(ABBBThrowable& Actor) const;
};
