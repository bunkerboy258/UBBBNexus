#pragma once

#include "BBBMonsterReplicationItem.h"

/** 单条传输记录 当前结果与生命周期分开调度 */
struct FBBBMonsterStreamRecord final
{
    enum class EKind : uint8
    {
        Birth,
        Motion,
        InitialDamage,
        Damage,
        Forget
    };

    FBBBMonsterReplicationItem Action;
    FBBBMonsterDamageTotals Damage;
    FString DefinitionPath;
    EKind Kind = EKind::Motion;

    bool Serialize(FArchive& Ar, UPackageMap* PackageMap);
};
