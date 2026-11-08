#pragma once
struct FBBBCharacterMovementFactContext;
/** 物理完成后让角色锚点跟随身体 不反向驱动物理网格 */
class FBBBCharacterRagdollFollowProcessor final
{
public:
    /** @param Context 移动领域上下文 @return 无 */
    void Update(FBBBCharacterMovementFactContext &Context) const;
};
