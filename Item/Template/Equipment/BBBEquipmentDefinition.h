
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBEquipmentDefinition.generated.h"
class ABBBEquipmentActor;
class UAnimMontage;
class UBBBEquipmentPoseFragment;

//装备可占用的角色槽位
UENUM(BlueprintType)
enum class EBBBEquipmentSlot : uint8
{
    MainHand UMETA(DisplayName = "Main Hand")
};

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

    //装备姿势配置插槽
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Equipment")
    TObjectPtr<UBBBEquipmentPoseFragment> EquipmentPoseFragment;
};
