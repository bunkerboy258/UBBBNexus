#pragma once

class UAnimMontage;
struct FBBBSMGUpdateContext;

/** 已成立动作的直接表现操作 */
class FBBBSMGPresentationProcessor final
{
public:
    /** @param Context	本次更新上下文 @param Montage	角色蒙太奇 @param bClear	是否清除此资源占用的槽 @return 无 */
    static void SubmitCharacterMontage(const FBBBSMGUpdateContext &Context, UAnimMontage *Montage, bool bClear = false);

    /** @param Context	本次更新上下文 @param Montage	装备蒙太奇 @return 无 */
    static void PlayEquipmentMontage(const FBBBSMGUpdateContext &Context, UAnimMontage *Montage);

    /** @param Context	本次更新上下文 @return 无 */
    static void PlayFire(const FBBBSMGUpdateContext &Context);
};
