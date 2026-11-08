#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Combat/FBBBHitContribution.h"
class APawn;

/** 等待投送至控制者的独立命中消息 */
struct FBBBCharacterDamageInboxState final
{
    /** 按提交顺序保存每个完整命中 */
    TArray<FBBBHitContribution> Hits;
    /** 传输序号与连接发送者 不参与玩法计算 */
    TArray<uint64> Sequences;
    TArray<TWeakObjectPtr<APawn>> Sources;
};
