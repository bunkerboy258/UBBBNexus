#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBItemRuntimeData.generated.h"

class UBBBFireRuntimeData;
class UBBBItemDefinition;
class UBBBMagazineRuntimeData;

/** 物品各操作域运行数据根 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBItemRuntimeData final : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 按物品显式操作域创建运行数据
     * @param Definition    物品静态配置
     */
    void Initialize(const UBBBItemDefinition &Definition);

    /** @return 开火操作域运行数据 */
    UBBBFireRuntimeData *GetFire() const;

    /** @return 弹匣操作域运行数据 */
    UBBBMagazineRuntimeData *GetMagazine() const;

private:
    /** 开火操作域运行数据 */
    UPROPERTY(Instanced)
    TObjectPtr<UBBBFireRuntimeData> Fire = nullptr;

    /** 弹匣操作域运行数据 */
    UPROPERTY(Instanced)
    TObjectPtr<UBBBMagazineRuntimeData> Magazine = nullptr;
};
