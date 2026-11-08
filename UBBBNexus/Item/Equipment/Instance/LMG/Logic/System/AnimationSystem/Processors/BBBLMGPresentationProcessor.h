#pragma once

class UAnimMontage;
struct FBBBLMGAnimationUpdateContext;

/** 已成立动作的直接表现操作 */
class FBBBLMGPresentationProcessor final
{
public:
    /**
     * @param Context	本次更新上下文
     * @param Montage	覆盖对应槽位的角色蒙太奇
     * @return 无
     */
    static void SubmitCharacterMontage(const FBBBLMGAnimationUpdateContext &Context, UAnimMontage *Montage);

    /** @param Context	本次更新上下文 @param Montage	装备蒙太奇 @return 无 */
    static void PlayEquipmentMontage(const FBBBLMGAnimationUpdateContext &Context, UAnimMontage *Montage);

    /** @param Context	本次更新上下文 @return 无 */
    static void PlayFire(const FBBBLMGAnimationUpdateContext &Context);
};
