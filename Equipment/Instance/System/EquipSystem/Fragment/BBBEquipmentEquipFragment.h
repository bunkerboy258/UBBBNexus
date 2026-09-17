#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentEquipFragment.generated.h"

class FBBBCharacterInput;

/** 装备挂接和握持配置的可选实现 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentEquipFragment
{
    GENERATED_BODY()

    virtual ~FBBBEquipmentEquipFragment() = default;

    /** @return 角色挂接装备时使用的相对变换 */
    virtual FTransform GetSpawnOffset() const
    {
        return FTransform::Identity;
    }

    /**
     * @param CharacterAPI 角色表现入口
     * @param Sequence    操作序号
     * @return 装备表现是否已提交
     */
    virtual bool SubmitMontage(FBBBCharacterInput &CharacterAPI, int32 Sequence) const
    {
        return false;
    }

    /** @return 左手握持插槽 */
    virtual FName GetLeftHandSocketName() const
    {
        return NAME_None;
    }

    /** @return 左手握持插槽偏移 */
    virtual FVector GetLeftHandSocketOffset() const
    {
        return FVector::ZeroVector;
    }

    /** @return 左手 IK 偏移 */
    virtual FVector GetLeftHandIKOffset() const
    {
        return FVector::ZeroVector;
    }
};
