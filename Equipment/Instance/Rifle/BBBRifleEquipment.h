#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Signature/BBBRifleSignature.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Runtime/BBBEquipmentRuntime.h"
#include "BBBRifleEquipment.generated.h"

class UBBBRifleDefinition;

/** 以静态步枪签名驱动的唯一步枪装备演员 */
UCLASS()
class ABBB_EVAC_API ABBBRifleEquipment final : public ABBBEquipment
{
    GENERATED_BODY()

public:
    /** 步枪实例私有运行时数据根 */
    using FRuntimeData = TBBBEquipmentRuntimeData<FBBBRifleSignature>;

    /** 步枪公开持有的唯一运行时聚合黑板 */
    FRuntimeData RuntimeData;

    /**
     * 将角色通用命令映射为步枪静态输入包
     * @param Command        角色提交的统一命令
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitCommand(const FBBBEquipmentCommand &Command, bool bInIsMirror) override;

protected:
    /**
     * 在装备动画工作完成后解析步枪输入帧
     * @param DeltaSeconds    帧间隔
     * @return 无
     */
    virtual void UpdateEquipment(float DeltaSeconds) override;

private:
    friend class FBBBCharacterEquipmentLifecycleProcessor;

    /**
     * 绑定步枪配置并初始化固定状态
     * @param InDefinition    步枪静态配置
     * @param InInstanceId    实例唯一标识
     * @param bInIsMirror     是否为远端镜像
     * @return 初始化是否成功
     */
    bool InitializeRifle(UBBBRifleDefinition &InDefinition, const FGuid &InInstanceId, bool bInIsMirror);

};
