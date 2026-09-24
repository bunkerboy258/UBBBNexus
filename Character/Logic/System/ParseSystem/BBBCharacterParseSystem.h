#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

class FBBBCharacterInitializer;
struct FBBBCharacterRuntimeData;

/** 角色包解析系统 负责将输入转换为领域黑板事实 */
class ABBB_EVAC_API FBBBCharacterParseSystem final
{
public:
    /** 解析当前固定输入帧 */
    void Update() const;

private:
    friend class FBBBCharacterInitializer;

    void Initialize(FBBBCharacterRuntimeData &InData);

    FBBBCharacterRuntimeData *Data = nullptr;
    FBBBCharacterInputProcessor InputProcessor;
};
