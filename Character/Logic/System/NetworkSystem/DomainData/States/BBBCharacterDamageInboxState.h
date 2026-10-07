#pragma once
#include "CoreMinimal.h"
class APawn;

/** 等待投送至控制者的独立命中消息 */
struct FBBBCharacterDamageInboxState final
{
    /** 每次命中的伤害 */
    TArray<float> Damages;
    /** 命中骨骼 */
    TArray<FName> Bones;
    /** 命中位置 */
    TArray<FVector> Positions;
    /** 命中方向 */
    TArray<FVector> Directions;
    /** 已拥有合法连接的发送角色 */
    TArray<TWeakObjectPtr<APawn>> Sources;
    /** 对应来源的命中序号 */
    TArray<uint64> Sequences;
};
