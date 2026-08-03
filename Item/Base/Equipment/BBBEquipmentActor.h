#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipmentActor.generated.h"

class FBBBCharacterExternalAPI;
class UBBBEquipmentDefinition;
class UBBBEquipmentInstance;
struct FBBBCharacterAnimationRequest;

UCLASS(Abstract)
class ABBB_EVAC_API ABBBEquipmentActor : public AActor
{
    GENERATED_BODY()

public:

    ABBBEquipmentActor();

    virtual void Initialize(UBBBEquipmentInstance &InEquipmentInstance, FBBBCharacterExternalAPI &InCharacterAPI);

    virtual void Tick(float DeltaSeconds) override;

    virtual void Equip();

    virtual bool Fire();

    virtual bool Reload();

    virtual void PresentFire();

    virtual void PresentReload();

    const UBBBEquipmentDefinition *GetEquipmentDefinition() const;

protected:

    void QueueMontage(const FBBBCharacterAnimationRequest &Request) const;

    UPROPERTY()
    TObjectPtr<UBBBEquipmentInstance> EquipmentInstance = nullptr;

    const UBBBEquipmentDefinition *EquipmentDefinition = nullptr;

    FBBBCharacterExternalAPI *CharacterAPI = nullptr;
};
