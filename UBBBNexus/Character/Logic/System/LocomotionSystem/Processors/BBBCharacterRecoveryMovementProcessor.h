#pragma once
struct FBBBCharacterLocomotionUpdateContext;
/** 在物理阶段变化时统一恢复胶囊与 CMC */
class FBBBCharacterRecoveryMovementProcessor final
{
public:
    /** @param Context 移动更新依赖 @return 无 */
    void Update(FBBBCharacterLocomotionUpdateContext &Context) const;
};
