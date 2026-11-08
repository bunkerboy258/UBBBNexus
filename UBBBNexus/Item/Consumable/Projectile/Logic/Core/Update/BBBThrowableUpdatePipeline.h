#pragma once
class ABBBThrowable;
/** 投掷物Update入口 */
class FBBBThrowableUpdatePipeline final
{
public:
    /** @param Actor	当前投掷物 @return 无 */
    void Update(ABBBThrowable& Actor) const;
};
