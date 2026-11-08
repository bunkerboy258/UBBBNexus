#pragma once

struct FBBBCharacterMovementFactContext;

/** 在 CMC 完成后发布来源无关的基本移动事实 */
class FBBBCharacterMovementFactProcessor final
{
public:
    /**
     * @param Context\t本次移动事实采集上下文
     * @return 无返回值
     */
    void Update(FBBBCharacterMovementFactContext &Context) const;
};
