#pragma once
/** 近战表现已消费的攻击结果 */
struct FBBBMeleeAnimationState final
{
    /** 已播放的攻击序号 */
    int32 AttackSequence = 0;
    /** 已发布的攻击状态 */
    bool bAttacking = false;
    /** 已接收第一份镜像结果 */
    bool bInitialized = false;
private:
    friend struct FBBBMeleeAnimationDomainState;
    FBBBMeleeAnimationState() = default;
};
