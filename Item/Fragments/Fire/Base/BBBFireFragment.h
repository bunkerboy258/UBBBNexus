
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/Fragment/BBBItemFragment.h"
#include "BBBFireFragment.generated.h"
class ABBBWeaponActor;
class UBBBFireRuntimeData;
class UBBBItemFragmentRuntimeData;
struct FBBBShotEvent;

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBFireFragment : public UBBBItemFragment
{
    GENERATED_BODY()
public:

    virtual UBBBItemFragmentRuntimeData *InitializeRuntimeData(UObject &Outer) const override;

    virtual bool Fire(
        ABBBWeaponActor &Weapon,
        UBBBFireRuntimeData &RuntimeData,
        FBBBShotEvent &OutShotEvent) const PURE_VIRTUAL(UBBBFireFragment::Fire, return false;);

    virtual void Present(
        ABBBWeaponActor &Weapon) const PURE_VIRTUAL(UBBBFireFragment::Present,);
};
