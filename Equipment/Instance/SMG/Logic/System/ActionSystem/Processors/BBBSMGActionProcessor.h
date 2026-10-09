#pragma once

struct FBBBSMGRuntimeData;
struct FBBBSMGUpdateContext;
class UBBBSMGDefinition;

/** 冲锋枪动作规则的唯一写入者 */
class FBBBSMGActionProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBSMGUpdateContext &Context);

    /** @param Data	冲锋枪运行时数据 @param Definition	冲锋枪静态配置 @return 无 */
    static void Initialize(FBBBSMGRuntimeData &Data, const UBBBSMGDefinition &Definition);

    /** @param Data	冲锋枪运行时数据 @return 无 */
    static void Stop(FBBBSMGRuntimeData &Data);

private:
    /** @param Context	本机枪口与控制权上下文 @return 无 */
    static void SpawnProjectile(FBBBSMGUpdateContext& Context);
};
