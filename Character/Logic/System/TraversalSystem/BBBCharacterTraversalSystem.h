#pragma once

class FBBBCharacterInitializer;

/** 本版本不提供攀爬行为的系统预留位置 */
class ABBB_EVAC_API FBBBCharacterTraversalSystem final
{
public:
    /** @return 无 保留主管线调用位置 */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /** @return 无 当前没有运行依赖或状态 */
    void Initialize();
};
