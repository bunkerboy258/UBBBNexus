#pragma once

struct FBBBCharacterEquipmentUpdateContext;

/** 将角色已经成立的操作许可交给抽象装备入口 */
class FBBBCharacterEquipmentActionPermissionProcessor final
{
public:
    /** @param Context	本次装备上下文 @return 无 */
    void Update(FBBBCharacterEquipmentUpdateContext &Context) const;
};
