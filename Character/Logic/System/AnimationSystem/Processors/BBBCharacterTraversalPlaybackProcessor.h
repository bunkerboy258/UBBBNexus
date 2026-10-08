#pragma once

struct FBBBCharacterAnimationUpdateContext;

/** 在固定槽消费后采集实际攀爬播放并交接根运动 */
class FBBBCharacterTraversalPlaybackProcessor final
{
public:
    /**
     * 同帧确认播放实例并在退出时保留状态过渡需要的尾姿
     * @param Context	动画领域更新上下文
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
