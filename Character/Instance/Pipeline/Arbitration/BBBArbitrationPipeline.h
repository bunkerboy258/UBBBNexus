#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterRuntimeData;
class FBBBCharacterInitializer;

/** 角色操作仲裁阶段 */
class FBBBArbitrationPipeline final
{
public:
    /** 推进当前阶段 */
    void Update(bool bRestoreMode) const;

private:
    friend class FBBBCharacterInitializer;
    FBBBCharacterRuntimeData *Data = nullptr;
};
