#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterDamageFragment.generated.h"

/** 已成立的玩家累计贡献与本轮受伤结果 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterDamageFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 键为玩家身份 值为对这一代小怪的累计伤害 */
    UPROPERTY()
    TMap<int32, double> Contributions;

    /** 已扣血的受伤事实 */
    bool bReceivedDamage = false;
};

/** 累计字典需要原生构造 拷贝与析构 明确接受 Mass 非平凡 Fragment */
template<>
struct TMassFragmentTraits<FBBBMonsterDamageFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
