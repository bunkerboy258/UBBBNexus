#pragma once

#include "Engine/DamageEvents.h"

/** 点伤害的公开表现数据 不向接收方暴露弹丸 Fragment */
struct FBBBProjectilePointDamageEvent final : public FPointDamageEvent
{
    static constexpr int32 ClassID = 0x424250;

    /** 独立于伤害和踉跄的伤口表现强度 */
    float WoundStrength = 1.0f;

    /** @return 当前事件类型 */
    virtual int32 GetTypeID() const override { return ClassID; }

    /** @param Type	查询类型 @return 同时保持标准点伤害的类型语义 */
    virtual bool IsOfType(int32 Type) const override { return Type == ClassID || FPointDamageEvent::IsOfType(Type); }
};
