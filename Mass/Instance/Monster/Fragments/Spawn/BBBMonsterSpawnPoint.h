#pragma once
#include "CoreMinimal.h"
class UBBBMonsterDefinition;

/** 已验收的单个出生点 不持有第二份玩法状态 */
struct FBBBMonsterSpawnPoint final
{
    /** 本次选中的模板 */
    TWeakObjectPtr<UBBBMonsterDefinition> Definition;
    /** 胶囊中心变换 */
    FTransform Transform = FTransform::Identity;
    /** 与网络身份分离的出生种子 */
    uint32 Seed = 1;
};
