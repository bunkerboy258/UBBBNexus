#pragma once

#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

struct FBBBRifleInputContext;

/** 步枪镜像实例还原已确认事实的输入 */
struct FBBBRifleRestoreFactInput final
{
    FBBBEquipmentActionFact Fact;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /** @param Context 解析上下文 @return 当前是否允许应用 */
    bool CanApply(const FBBBRifleInputContext &Context) const;

    /** @param Context 解析上下文 @return 无 */
    void Apply(FBBBRifleInputContext &Context) const;
};
