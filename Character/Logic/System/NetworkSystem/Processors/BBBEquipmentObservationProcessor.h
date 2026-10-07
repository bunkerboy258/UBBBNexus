#pragma once

struct FBBBCharacterNetworkUpdateContext;

/** 只观察实际持有关系与调度公共消息投递 */
class FBBBEquipmentObservationProcessor final
{
public:
    /** @param Context	本帧角色网络结果 @return 无 */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
