#pragma once

struct FBBBRevolverRuntimeData;
struct FBBBRevolverUpdateContext;
class UBBBRevolverDefinition;

/** 左轮动作规则的唯一写入者 */
class FBBBRevolverActionProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBRevolverUpdateContext &Context);

    /** @param Data	左轮运行时数据 @param Definition	左轮静态配置 @return 无 */
    static void Initialize(FBBBRevolverRuntimeData &Data, const UBBBRevolverDefinition &Definition);

    /** @param Data	左轮运行时数据 @return 无 */
    static void Stop(FBBBRevolverRuntimeData &Data);

private:
    /** @param Context	本机枪口与控制权上下文 @return 无 */
    static void SpawnProjectile(FBBBRevolverUpdateContext& Context);
};
