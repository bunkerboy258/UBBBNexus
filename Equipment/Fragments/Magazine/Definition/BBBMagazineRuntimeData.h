#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBMagazineRuntimeData.generated.h"

struct FBBBMagazineFragment;
class ABBBMagazinePresentationActor;

/** 弹匣操作域运行数据 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMagazineRuntimeData final : public UObject
{
    GENERATED_BODY()

public:
private:
    friend struct FBBBMagazineFragment;

    /** 当前弹匣弹药 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 MagazineAmmo = 0;

    /** 当前备用弹药 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 ReserveAmmo = 0;

    /** 当前安装在武器上的弹匣表现实体 */
    UPROPERTY()
    TObjectPtr<ABBBMagazinePresentationActor> LoadedMagazineActor = nullptr;
};
