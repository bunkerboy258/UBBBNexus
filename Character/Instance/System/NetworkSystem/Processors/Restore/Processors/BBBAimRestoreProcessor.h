
#pragma once
#include "CoreMinimal.h"
struct FBBBAimNetworkState;
struct FBBBAimRuntimeData;

class ABBB_EVAC_API FBBBAimRestoreProcessor final
{
public:

    /**
     * 将远端瞄准状态还原到本地瞄准运行时数据
     * @param AimData	瞄准运行时数据
     * @param AimState	远端瞄准网络状态
     */
    void Update(
        FBBBAimRuntimeData &AimData,
        const FBBBAimNetworkState &AimState) const;
};
