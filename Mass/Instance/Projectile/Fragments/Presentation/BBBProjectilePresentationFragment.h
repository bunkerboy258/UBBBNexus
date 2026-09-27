#pragma once

#include "MassEntityTypes.h"
#include "BBBProjectilePresentationFragment.generated.h"

class UNiagaraDataChannelAsset;

/** 子弹批量表现通道 */
USTRUCT()
struct ABBB_EVAC_API FBBBProjectilePresentationFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 批量光效通道 */
    UPROPERTY()
    TWeakObjectPtr<UNiagaraDataChannelAsset> Channel;

};
