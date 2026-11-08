#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/States/BBBCharacterPhysicalPresentationState.h"

/** 身体碰撞输入 只贡献接触事实 */
struct FBBBCharacterFallContactLocalControlPacket final
{
    /** 同帧实际接触 */
    TArray<FBBBCharacterFallContact> Contacts;
    /** @return 接触数值是否有效 */
    bool IsValid() const
    {
        return !Contacts.IsEmpty() && !Contacts.ContainsByPredicate([](const auto& Contact)
        {
            return Contact.Episode == 0 || !FMath::IsFinite(Contact.DownSpeed) || Contact.DownSpeed < 0
                || Contact.Hit.ImpactNormal.ContainsNaN() || Contact.Hit.ImpactPoint.ContainsNaN();
        });
    }
    /** @param Context 当前事实 @return 是否接受本机碰撞 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror;
    }
    /** @param Context 接触收件状态 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.PhysicalPresentation.FallContacts.Append(Contacts);
    }
};
