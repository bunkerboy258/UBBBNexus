#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/Events/BBBEquipmentActionPresentation.h"
#include "BBBFireDomin.generated.h"

class ABBBEquipmentPresentationActor;
struct FBBBEquipmentFireResult;
class UBBBFireRuntimeData;

/** 开火操作域基类 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBFireDomin
{
    GENERATED_BODY()

public:
    virtual ~FBBBFireDomin() = default;

    /**
     * 创建开火操作域运行数据
     * @param Outer	运行数据所有者
     * @return 创建完成的运行数据
     */
    virtual UBBBFireRuntimeData *InitializeRuntimeData(UObject &Outer) const;

    /**
     * 执行一次开火
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData			开火操作域运行数据
     * @param RuntimeData			开火操作域运行数据
     * @return 是否成功开火
     */
    virtual bool Fire(
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBFireRuntimeData &RuntimeData,
        FBBBEquipmentFireResult &OutResult) const;

    /**
     * 播放一次开火表现
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     */
    virtual void Present(
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBFireRuntimeData &RuntimeData) const;

    /**
     * 构造一次开火动作的人物表现数据
     * @param OutPresentation 接收开火动作表现数据
     * @return 无
     */
    virtual void BuildFireActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const;

};
