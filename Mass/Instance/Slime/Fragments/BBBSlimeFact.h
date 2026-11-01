#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Config/BBBSlimeDefinition.h"
#include "BBBSlimeFact.generated.h"

/** 粘液当前结果 飞行零 地面一 */
USTRUCT()
struct ABBB_EVAC_API FBBBSlimeFact final
{
    GENERATED_BODY()

    /** 本次粘液的独立身份 */
    UPROPERTY()
    FGuid InstanceId;
    /** 生成来源 仅用于额度 不约束存活 */
    UPROPERTY()
    FGuid SourceId;
    /** 当前结果版本 */
    UPROPERTY()
    uint32 Revision = 0;
    /** 当前规则 */
    UPROPERTY()
    TObjectPtr<UBBBSlimeDefinition> Definition;
    /** 当前飞行或地面阶段 */
    UPROPERTY()
    uint8 Phase = 0;
    /** 当前世界位置 */
    UPROPERTY()
    FVector Location = FVector::ZeroVector;
    /** 当前飞行速度 */
    UPROPERTY()
    FVector Velocity = FVector::ZeroVector;
    /** 地面法线 */
    UPROPERTY()
    FVector Normal = FVector::UpVector;
    /** 本阶段剩余生命周期 */
    UPROPERTY()
    float EndsAt = 0;

    /** @return 当前结果是否可用于还原 */
    bool IsValid() const
    {
        return InstanceId.IsValid() && SourceId.IsValid() && Revision > 0 && Definition && Definition->IsValid()
            && Phase <= 1 && !Location.ContainsNaN() && !Velocity.ContainsNaN() && !Normal.ContainsNaN()
            && FMath::IsFinite(EndsAt) && Normal.IsNormalized();
    }
};
