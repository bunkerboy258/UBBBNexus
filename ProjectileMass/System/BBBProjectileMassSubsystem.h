#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "BBBProjectileMassSubsystem.generated.h"

struct FBBBProjectileSpawnRequest;

/** 将本机确认的开火请求转换为轻量Mass实体弹丸 */
UCLASS()
class ABBB_EVAC_API UBBBProjectileMassSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    /**
     * 使用请求中的枪口变换创建并初始化一枚Mass弹丸
     * @param Request	已确认开火的弹丸出生请求
     * @return 是否成功创建弹丸实体
     */
    bool SubmitSpawnRequest(const FBBBProjectileSpawnRequest& Request);
};
