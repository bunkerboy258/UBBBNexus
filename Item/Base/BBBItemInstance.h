#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBItemInstance.generated.h"

class UBBBItemDefinition;
class UBBBItemRuntimeData;

/** 物品实例根对象 */
UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API UBBBItemInstance : public UObject
{
    GENERATED_BODY()

public:

    const FGuid &GetInstanceId() const;

    UBBBItemDefinition *GetDefinition() const;

    UBBBItemRuntimeData *GetRuntimeData() const;

    int32 GetStackCount() const;

    bool IsValid() const;

protected:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemRuntimeData> RuntimeData = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 StackCount = 1;
};
