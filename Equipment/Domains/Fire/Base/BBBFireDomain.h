#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDomain.h"
#include "BBBFireDomain.generated.h"

class ABBBEquipmentPresentationActor;
class FBBBCharacterExternalAPI;
class UBBBFireRuntimeData;

/** 开火领域接口 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBFireDomain : public UBBBEquipmentDomain
{
    GENERATED_BODY()

public:
    /**
     * 创建开火领域运行数据
     * @param Outer	运行数据生命周期所有者
     * @return 创建完成的开火领域运行数据
     */
    virtual UBBBFireRuntimeData *InitializeRuntimeData(UObject &Outer) const;

    /**
     * 响应一次本地开火
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData		开火领域运行数据
     * @return 是否成功开火
     */
    virtual bool Fire(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBFireRuntimeData &RuntimeData) const PURE_VIRTUAL(UBBBFireDomain::Fire, return false;);

    /**
     * 响应一次远端开火表现
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     */
    virtual void Present(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor) const PURE_VIRTUAL(UBBBFireDomain::Present,);
};
