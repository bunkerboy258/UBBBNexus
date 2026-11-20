#pragma once

class ABBBThrowable;

/** 更新起爆后独立持续的区域 */
class FBBBThrowableAreaProcessor final
{
public:
    /** @param Actor	当前投掷物 @return 无 */
    void Update(ABBBThrowable& Actor) const;
};
