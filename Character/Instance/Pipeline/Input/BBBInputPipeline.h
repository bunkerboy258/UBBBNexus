#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterRuntimeData;
class FBBBCharacterInitializer;

/** 角色输入整理阶段 */
class FBBBInputPipeline final
{
public:
    /** 推进当前阶段 */
    void Update() const;

private:
    friend class FBBBCharacterInitializer;
    FBBBCharacterRuntimeData *Data = nullptr;
};
