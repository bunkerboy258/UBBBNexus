#pragma once
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/States/BBBWorldDropState.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/States/BBBWorldDropPresentationState.h"
#include "BBBWorldDropDomainState.generated.h"
/** 掉落物业务领域的唯一状态持有者 */
USTRUCT()
struct FBBBWorldDropDomainState final
{
    GENERATED_BODY()
public:
    /** @return 本地显示资源的只读状态 */
    const FBBBWorldDropPresentationState &ReadWorldDropPresentationState() const
    {
        return PresentationState;
    }
    /** @return 世界掉落物的只读当前业务状态 */
    const FBBBWorldDropState &ReadWorldDropState() const
    {
        return DropState;
    }
private:
    friend class FBBBWorldDropPresentationProcessor;
    /** 光柱与白色图标的显示资源 */
    UPROPERTY()
    FBBBWorldDropPresentationState PresentationState;
    friend class FBBBWorldDropInitializer;
    friend class FBBBWorldDropParseSystem;
    friend class FBBBWorldDropProcessor;
    friend class FBBBWorldDropShutdown;
    /** 业务状态与待消费输入 */
    UPROPERTY()
    FBBBWorldDropState DropState;
};
