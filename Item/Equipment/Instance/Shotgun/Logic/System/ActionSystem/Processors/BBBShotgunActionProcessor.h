#pragma once

struct FBBBShotgunRuntimeData;
struct FBBBShotgunActionUpdateContext;
class UBBBShotgunDefinition;

/** 霰弹枪动作规则的唯一写入者 */
class FBBBShotgunActionProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBShotgunActionUpdateContext &Context);

    /** @param Data	霰弹枪运行时数据 @param Definition	霰弹枪静态配置 @return 无 */
    static void Initialize(FBBBShotgunRuntimeData &Data, const UBBBShotgunDefinition &Definition);

    /** @param Data	霰弹枪运行时数据 @return 无 */
    static void Stop(FBBBShotgunRuntimeData &Data);

private:
    /** @param Context	本机枪口与控制权上下文 @return 无 */
    static void SpawnProjectile(FBBBShotgunActionUpdateContext& Context);
};
