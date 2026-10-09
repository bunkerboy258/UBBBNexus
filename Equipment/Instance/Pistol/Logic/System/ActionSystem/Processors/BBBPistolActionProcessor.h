#pragma once

struct FBBBPistolRuntimeData;
struct FBBBPistolUpdateContext;
class UBBBPistolDefinition;

/** 手枪动作规则的唯一写入者 */
class FBBBPistolActionProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBPistolUpdateContext &Context);

    /** @param Data	手枪运行时数据 @param Definition	手枪静态配置 @return 无 */
    static void Initialize(FBBBPistolRuntimeData &Data, const UBBBPistolDefinition &Definition);

    /** @param Data	手枪运行时数据 @return 无 */
    static void Stop(FBBBPistolRuntimeData &Data);

private:
    /** @param Context	本机枪口与控制权上下文 @return 无 */
    static void SpawnProjectile(FBBBPistolUpdateContext& Context);
};
