
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBEquipmentDefinition.generated.h"
class ABBBEquipmentActor;
class UAnimMontage;

UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API UBBBEquipmentDefinition : public UBBBItemDefinition
{
    GENERATED_BODY()
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    EBBBEquipmentSlot EquipmentSlot = EBBBEquipmentSlot::MainHand;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TSubclassOf<ABBBEquipmentActor> EquipmentActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FTransform SpawnOffset = FTransform::Identity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation")
    TObjectPtr<UAnimMontage> EquipMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation")
    TObjectPtr<UAnimMontage> HoldIdleMontage = nullptr;
};
