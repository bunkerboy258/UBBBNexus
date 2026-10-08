#pragma once

struct FBBBCharacterLifeUpdateContext;

/** 维护角色救援的当前结果还原 */
class FBBBCharacterRescueMirrorProcessor final
{
public:
    /** @param Context	本次生命上下文 @return 无 */
    void Update(FBBBCharacterLifeUpdateContext &Context) const;
};
