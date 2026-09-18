#pragma once

class FBBBCharacterNetworkSystem;
struct FBBBCharacterEquipmentEvents;

/** 上传本帧装备动作事件 */
class FBBBEquipmentActionUploadProcessor final
{
public:
    /**
     * 上传本帧动作
     * @param EquipmentEvents 装备事件
     * @param NetworkSystem 网络系统
     */
    void Update(
        const FBBBCharacterEquipmentEvents &EquipmentEvents,
        FBBBCharacterNetworkSystem &NetworkSystem) const;
};
