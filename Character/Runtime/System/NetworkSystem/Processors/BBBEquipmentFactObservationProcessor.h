#pragma once

struct FBBBCharacterNetworkUpdateContext;

/** 上传本帧装备动作事件 */
class FBBBEquipmentFactObservationProcessor final
{
public:
    /**
     * 上传本帧已经成立的装备事实
     * @param Context 本次网络更新上下文
     * @return 无
     */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
