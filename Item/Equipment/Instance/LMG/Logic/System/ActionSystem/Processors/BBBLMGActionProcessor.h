#pragma once

struct FBBBLMGRuntimeData;
struct FBBBLMGUpdateContext;
class UBBBLMGDefinition;

/** 轻机枪动作规则的唯一写入者 */
class FBBBLMGActionProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBLMGUpdateContext &Context);

    /** @param Data	轻机枪运行时数据 @param Definition	轻机枪静态配置 @return 无 */
    static void Initialize(FBBBLMGRuntimeData &Data, const UBBBLMGDefinition &Definition);

    /** @param Data	轻机枪运行时数据 @return 无 */
    static void Stop(FBBBLMGRuntimeData &Data);

private:
    /** @param Context	本机枪口与控制权上下文 @return 无 */
    static void SpawnProjectile(FBBBLMGUpdateContext& Context);
};
