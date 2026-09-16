#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "BBBEquipmentSkeletalPresentationActor.generated.h"

class USkeletalMeshComponent;

/** 使用装备骨骼网格和装备动画实例的表现实体 */
UCLASS()
class ABBB_EVAC_API ABBBEquipmentSkeletalPresentationActor : public ABBBEquipmentPresentationActor
{
    GENERATED_BODY()

public:
    /** 构造装备骨骼表现组件 */
    ABBBEquipmentSkeletalPresentationActor();

    /** @return 装备骨骼网格 */
    USkeletalMeshComponent *GetEquipmentSkeletalMesh() const;

    /** @return 装备姿态与插槽使用的骨骼网格 */
    virtual USceneComponent *GetEquipmentAttachmentComponent() const override;

private:
    /** 装备骨骼显示网格 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> EquipmentSkeletalMesh;
};
