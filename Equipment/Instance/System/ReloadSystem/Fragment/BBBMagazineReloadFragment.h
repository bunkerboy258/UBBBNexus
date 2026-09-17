#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Fragment/BBBEquipmentReloadFragment.h"
#include "BBBMagazineReloadFragment.generated.h"

class UAnimMontage;

/** 动画关键帧驱动的弹夹换弹实现 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMagazineReloadFragment final : public FBBBEquipmentReloadFragment
{
    GENERATED_BODY()

    virtual bool CanReload(int32 LoadedAmmo, int32 AmmoCapacity) const override;
    virtual bool Begin(FBBBEquipmentReloadContext &Context) const override;
    virtual bool DetachMagazine(FBBBEquipmentReloadContext &Context) const override;
    virtual bool LoadMagazine(FBBBEquipmentReloadContext &Context) const override;
    virtual bool Cancel(FBBBEquipmentReloadContext &Context) const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Reload")
    TObjectPtr<UAnimMontage> Montage = nullptr;
};
