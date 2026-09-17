
#pragma once
#include "CoreMinimal.h"
struct FBBBInputRawData;
struct FBBBInputRuntimeData;
struct FBBBInputPipelineConfig;
struct FBBBRawInputFrame;

class ABBB_EVAC_API FBBBInputProcessor final
{
public:

    /**
     * 将原始输入规范化为处理后输入帧并提交
     * @param InputRawData	原始输入缓冲
     * @param InputData	输入运行时数据
     * @param Config	输入管线配置
     * @param DeltaSeconds	帧间隔秒数
     */
    void Update(
        const FBBBInputRawData &InputRawData,
        FBBBInputRuntimeData &InputData,
        const FBBBInputPipelineConfig &Config,
        float DeltaSeconds) const;
};
