#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterRuntimeData;
class FBBBCharacterInitializer;

/** 角色黑板应用阶段 */
class FBBBExecutionPipeline final
{
public:
    /** 推进当前阶段 */
    void Update() const;

private:
    friend class FBBBCharacterInitializer;
    FBBBCharacterRuntimeData *Data = nullptr;
};
