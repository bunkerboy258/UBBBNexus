#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipmentPresentationActor.generated.h"

class UArrowComponent;
class UStaticMeshComponent;

/** 装备在虚幻世界中的纯表现实体 */
UCLASS()
class ABBB_EVAC_API ABBBEquipmentPresentationActor : public AActor
{
    GENERATED_BODY()

public:
    /** 构造装备表现组件 */
    ABBBEquipmentPresentationActor();

    /** @return 装备显示网格 */
    UStaticMeshComponent *GetEquipmentMesh() const;

private:
    /** 装备组件根 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> EquipmentRoot;

    /** 装备显示网格 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> EquipmentMesh;
};
