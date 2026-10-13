#pragma once
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassInputSlot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorInitializeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorStartLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorPauseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorResumeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorStopLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorTriggerLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorQueryResultLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorCreatedLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/AuthorityFact/Director/FBBBMonsterDirectorStateAuthorityFactPacket.h"
#include "BBBMonsterDirectorInputFragment.generated.h"

/** 每类导演输入独立覆盖槽 */
USTRUCT()
struct FBBBMonsterDirectorInputFragment final : public FMassFragment
{
    GENERATED_BODY()
    /** Initialize 输入覆盖槽 */
    TBBBMassInputSlot<FBBBMonsterDirectorInitializeLocalControlPacket> Initialize;
    /** Start 输入覆盖槽 */
    TBBBMassInputSlot<FBBBMonsterDirectorStartLocalControlPacket> Start;
    /** Pause 输入覆盖槽 */
    TBBBMassInputSlot<FBBBMonsterDirectorPauseLocalControlPacket> Pause;
    /** Resume 输入覆盖槽 */
    TBBBMassInputSlot<FBBBMonsterDirectorResumeLocalControlPacket> Resume;
    /** Stop 输入覆盖槽 */
    TBBBMassInputSlot<FBBBMonsterDirectorStopLocalControlPacket> Stop;
    /** Trigger 输入覆盖槽 */
    TBBBMassInputSlot<FBBBMonsterDirectorTriggerLocalControlPacket> Trigger;
    /** QueryResult 输入覆盖槽 */
    TBBBMassInputSlot<FBBBMonsterDirectorQueryResultLocalControlPacket> QueryResult;
    /** Created 输入覆盖槽 */
    TBBBMassInputSlot<FBBBMonsterDirectorCreatedLocalControlPacket> Created;
    /** 主机当前结果覆盖槽 */
    TBBBMassInputSlot<FBBBMonsterDirectorStateAuthorityFactPacket> State;

    /**
     * @return 对应类型的唯一输入槽
     */
    template<typename TPacket>
    auto& GetSlot()
    {
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorInitializeLocalControlPacket>)
        {
            return Initialize;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorStartLocalControlPacket>)
        {
            return Start;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorPauseLocalControlPacket>)
        {
            return Pause;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorResumeLocalControlPacket>)
        {
            return Resume;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorStopLocalControlPacket>)
        {
            return Stop;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorTriggerLocalControlPacket>)
        {
            return Trigger;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorQueryResultLocalControlPacket>)
        {
            return QueryResult;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorCreatedLocalControlPacket>)
        {
            return Created;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorStateAuthorityFactPacket>)
        {
            return State;
        }
    }
};

/** 当前申请与配置副本需要原生构造 拷贝和析构 */
template<>
struct TMassFragmentTraits<FBBBMonsterDirectorInputFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
