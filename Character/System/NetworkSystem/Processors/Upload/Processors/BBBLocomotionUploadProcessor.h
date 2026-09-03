#pragma once

#include "CoreMinimal.h"

class FBBBCharacterNetworkSystem;
struct FBBBCharacterLocomotionRuntimeData;
struct FBBBNetworkRuntimeData;

/** 将本地移动步态变化上传到权威端 */
class ABBB_EVAC_API FBBBLocomotionUploadProcessor final
{
public:
    /**
     * 上传发生变化的移动步态
     * @param LocomotionData 移动运行时数据
     * @param NetworkData 网络运行时数据
     * @param NetworkSystem 角色网络系统
     */
    void Update(
        const FBBBCharacterLocomotionRuntimeData &LocomotionData,
        FBBBNetworkRuntimeData &NetworkData,
        FBBBCharacterNetworkSystem &NetworkSystem) const;
};
