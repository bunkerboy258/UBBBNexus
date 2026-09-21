#pragma once
struct FBBBCharacterEquipmentUpdateContext;

/** 转发角色装备命令并整理装备已确认结果 */
class FBBBCharacterEquipmentActionProcessor final
{
public:
    /**
     * 转发已批准输入
     * @param Commands	角色命令
     * @param State          角色装备选择
     * @param bIsMirror      是否只能投递网络确认事实
     * @return 无
     */
    /**
     * 转发本帧已经批准的装备命令
     * @param Context 本次装备更新上下文
     * @return 无
     */
    void Update(FBBBCharacterEquipmentUpdateContext &Context) const;

};
