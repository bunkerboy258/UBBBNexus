#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Continuous/BBBCharacterContinuousInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Discrete/BBBCharacterDiscreteInput.h"
#include "BBBWork/UBBBNexus/Character/Input/RestoreDiscrete/BBBCharacterRestoreDiscreteInput.h"

struct FBBBCharacterInputRuntimeData;
class FBBBCharacterInitializer;
class UBBBCharacterNetworkComponent;

/** 角色的输入入口 */
class ABBB_EVAC_API FBBBCharacterInput final
{
public:
    /**
     * 提交外部持续状态
     * @param Packet	持续状态数据
     * @return 是否接受输入
     */
    bool Submit(const FBBBCharacterContinuousInput &Packet);

    /**
     * 提交外部离散动作或装备事实
     * @param Packet	离散动作数据
     * @return 是否接受输入
     */
    bool Submit(FBBBCharacterDiscreteInput Packet);

private:
    friend class FBBBCharacterInitializer;
    friend class UBBBCharacterNetworkComponent;

    void Initialize(FBBBCharacterInputRuntimeData &Data);

    void SubmitRestore(FBBBCharacterRestoreDiscreteInput Packet);

    FBBBCharacterInputRuntimeData *Data = nullptr;
};
