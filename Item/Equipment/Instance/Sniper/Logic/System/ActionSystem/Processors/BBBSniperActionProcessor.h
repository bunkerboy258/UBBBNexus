#pragma once

struct FBBBSniperRuntimeData;
struct FBBBSniperUpdateContext;
class UBBBSniperDefinition;

/** 狙击枪动作规则的唯一写入者 */
class FBBBSniperActionProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBSniperUpdateContext &Context);

    /** @param Data	狙击枪运行时数据 @param Definition	狙击枪静态配置 @return 无 */
    static void Initialize(FBBBSniperRuntimeData &Data, const UBBBSniperDefinition &Definition);

    /** @param Data	狙击枪运行时数据 @return 无 */
    static void Stop(FBBBSniperRuntimeData &Data);

private:
    /** @param Context	本机枪口与控制权上下文 @return 无 */
    static void SpawnProjectile(FBBBSniperUpdateContext& Context);
};
