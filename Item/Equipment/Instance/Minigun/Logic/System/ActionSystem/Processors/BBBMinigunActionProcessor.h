#pragma once

struct FBBBMinigunRuntimeData;
struct FBBBMinigunUpdateContext;
class UBBBMinigunDefinition;

/** 转管机枪动作规则的唯一写入者 */
class FBBBMinigunActionProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBMinigunUpdateContext &Context);

    /** @param Data	转管机枪运行时数据 @param Definition	转管机枪静态配置 @return 无 */
    static void Initialize(FBBBMinigunRuntimeData &Data, const UBBBMinigunDefinition &Definition);

    /** @param Data	转管机枪运行时数据 @return 无 */
    static void Stop(FBBBMinigunRuntimeData &Data);

private:
    /** @param Context	本机枪口与控制权上下文 @return 无 */
    static void SpawnProjectile(FBBBMinigunUpdateContext& Context);
};
