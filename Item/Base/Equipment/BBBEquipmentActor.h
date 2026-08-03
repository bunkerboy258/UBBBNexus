
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemTypes.h"
#include "BBBEquipmentActor.generated.h"
struct FBBBCharacterAnimationRequest;
class FBBBCharacterExternalAPI;
class UBBBEquipmentDefinition;

UCLASS(Abstract)
class ABBB_EVAC_API ABBBEquipmentActor : public AActor
{
    GENERATED_BODY()
public:

    ABBBEquipmentActor();

    virtual void InitializeRuntimeEquipment(const FBBBItemInstance &InItemInstance, FBBBCharacterExternalAPI &InCharacterAPI);

    virtual void InitializeEquipmentMirror(const UBBBEquipmentDefinition &InDefinition, FBBBCharacterExternalAPI &InCharacterAPI);

    virtual void Tick(float DeltaSeconds) override;

    virtual void Equip();

    virtual bool Fire();

    virtual bool Reload();

    virtual void PresentFire();

    virtual void PresentReload();

    const UBBBEquipmentDefinition *GetEquipmentDefinition() const;

    EBBBEquipmentInstanceMode GetInstanceMode() const;
protected:

    void InitializeCommon(
        const UBBBEquipmentDefinition &InDefinition,
        FBBBCharacterExternalAPI &InCharacterAPI,
        EBBBEquipmentInstanceMode InInstanceMode);

    void QueueMontage(const FBBBCharacterAnimationRequest &Request) const;

    UPROPERTY()
    FBBBItemInstance ItemInstance;

    const UBBBEquipmentDefinition *EquipmentDefinition = nullptr;

    FBBBCharacterExternalAPI *CharacterAPI = nullptr;

    EBBBEquipmentInstanceMode InstanceMode = EBBBEquipmentInstanceMode::Runtime;
};
