#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/Operation/BBBItemOperation.h"
#include "BBBFireOperation.generated.h"

class ABBBEquipmentActor;
class UBBBFireRuntimeData;

/** 开火操作域接口 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBFireOperation : public UBBBItemOperation
{
    GENERATED_BODY()

public:
    /**
     * 创建开火操作域运行数据
     * @param Outer     运行数据生命周期所有者
     * @return 创建完成的开火运行数据
     */
    virtual UBBBFireRuntimeData *InitializeRuntimeData(UObject &Outer) const;

    /**
     * 响应一次完整开火操作
     * @param EquipmentActor    装备实体
     * @param RuntimeData       开火运行数据
     * @return 是否成功开火
     */
    virtual bool Fire(
        ABBBEquipmentActor &EquipmentActor,
        UBBBFireRuntimeData &RuntimeData) const PURE_VIRTUAL(UBBBFireOperation::Fire, return false;);

    /**
     * 响应一次远端开火表现
     * @param EquipmentActor    装备实体
     */
    virtual void Present(ABBBEquipmentActor &EquipmentActor) const PURE_VIRTUAL(UBBBFireOperation::Present,);
};
