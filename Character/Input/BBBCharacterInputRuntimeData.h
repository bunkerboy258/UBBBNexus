#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterPacketRegistry.h"
#include "BBBWork/UBBBNexus/Character/Input/States/BBBCharacterInputStates.h"
#include "BBBCharacterInputRuntimeData.generated.h"

class FBBBCharacterInput;
class FBBBCharacterParseSystem;
class FBBBCharacterInputProcessor;

/** 角色输入入口的持续状态快照和离散包收件箱 */
USTRUCT()
struct FBBBCharacterInputRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterInput;
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterInputProcessor;

    UPROPERTY()
    FBBBCharacterInputStates States;

    // 包队列不参与反射 包内资产指针由装备配置持有 单帧内消费完毕
    TArray<FBBBCharacterPacket> Pending;
};
