
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Item/BBBItemInstance.h"
#include "BBBEquipmentActor.generated.h"
struct FBBBCharacterAnimationRequest;
class FBBBCharacterExternalAPI;

UCLASS(Abstract)
class ABBB_EVAC_API ABBBEquipmentActor : public AActor
{
    GENERATED_BODY()
public:

    ABBBEquipmentActor();

    virtual void InitializeEquipment(const FBBBItemInstance &InItemInstance, FBBBCharacterExternalAPI &InCharacterAPI);

    virtual void Tick(float DeltaSeconds) override;

    virtual void Equip();
protected:

    void QueueMontage(const FBBBCharacterAnimationRequest &Request) const;

    UPROPERTY()
    FBBBItemInstance ItemInstance;

    FBBBCharacterExternalAPI *CharacterAPI = nullptr;
};
