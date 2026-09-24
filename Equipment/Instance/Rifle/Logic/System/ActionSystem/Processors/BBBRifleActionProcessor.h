#pragma once

struct FBBBRifleRuntimeData;
struct FBBBRifleUpdateContext;
class UBBBRifleDefinition;

/** 步枪动作规则的唯一写入者 */
class FBBBRifleActionProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBRifleUpdateContext &Context);

    /** @param Data	步枪运行时数据 @param Definition	步枪静态配置 @return 无 */
    static void Initialize(FBBBRifleRuntimeData &Data, const UBBBRifleDefinition &Definition);

    /** @param Data	步枪运行时数据 @return 无 */
    static void Stop(FBBBRifleRuntimeData &Data);
};
